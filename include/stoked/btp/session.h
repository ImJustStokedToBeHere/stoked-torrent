#pragma once
#include <stoked/btp/engine.h>
#include <stoked/btp/torrent.h>
#include <stoked/btp/peer.h>
#include <stoked/btp/peer_manager.h>
#include <stoked/btp/tracker.h>
#include <stoked/btp/udp_tracker.h>
#include <stoked/btp/bitset.h>
#include <stoked/btp/sys.h>
#include <string_view>
#include <vector>
#include <memory>
#include <array>
#include <iostream>
#include <set>

namespace stoked::btp {
    class tracker;

    struct session_settings {
        std::string torrent_filename{};
        std::string download_folder{};
        bool create_subfolder{true};
        uint8_t priority{0};
        bool use_pex{true};
        bool use_dht{true};
        short upload_slot_count{0};
        short padding{0};
    };

    struct session /*: public std::enable_shared_from_this<session>*/ {
        enum class state : char {            
            running,                        
            errored
        };

        enum class status : char {
            unknown,
            idle,
            init,
            starting_trackers,
            gathering_peers,
            downloading,
            seeding            
        };

        // JOHNS GOOGLE NUMBER: 5088590502    

        static session_settings default_session_settings(std::string filename = std::string{}) {
            return session_settings{
                std::move(filename),
                utils::module_info::get_file_name_from_module(),
                true,
                0,
                true,
                true,
                0,
                0
            };
        }

        explicit session(const session_settings& settings)
            :session(settings, torrent::load_torrent_file(settings.torrent_filename)) {
        }

        session(const session&) = delete;
        session(session&&) = default;

        session(session_settings settings, std::shared_ptr<torrent> meta)
            : _settings{std::move(settings)},
            _meta{meta}
        {
            std::string ver{"-S_" + utils::module_info::get_file_version_info_value(utils::file_version_info_value::product_version) + "-"};
            utils::replace_substr(ver, ".", "", false, false);
            std::copy(ver.begin(), ver.end(), _peer_id.data());
            utils::generate_random_byte_str(_peer_id.data(), ver.length(), 20 - ver.length());            
        }


        session& operator=(const session&) = delete;
        session& operator=(session&&) = default;

        bool start() {
            auto result = init_trackers() && gather_peers();
            // _announce_processor.start();
            return result;
        }

        bool init_trackers() {
            _status = status::starting_trackers;            
            std::set<std::string> url_list{};

            for (const auto& url : _meta->announce_list()) {
                if (url_list.find(url) == url_list.end()) {
                    url_list.insert(url);
                    auto trkr = tracker::create_tracker(url);
                    _tracker_map.emplace(trkr->url(), trkr);
                }
            }

            _status = status::idle;
            return true;
        }

        bool gather_peers() {
            _status = status::gathering_peers;

            std::vector<std::pair<std::weak_ptr<tracker>, scrape_result>> scrapes;

            for (auto const& [url, trkr] : _tracker_map) {

                if (trkr->server_is_down()) {
                    continue;
                }

                OutputDebugString(utils::make_str("scraping server: ", trkr->url(), "\n").c_str());

                scrape_result res = trkr->scrape(this);

                OutputDebugString(utils::make_str("server: ", trkr->url()
                                                  , ", d: ", res.completed_downloads
                                                  , " s: ", res.connected_seeds
                                                  , " l: ", res.connected_leeches, "\n").c_str());

                scrapes.push_back(std::make_pair(trkr, res));
            }


            for (auto const& [hnd, res] : scrapes) {
                auto trkr{hnd.lock()};

                _completed_downloads += res.completed_downloads;
                _connected_seeds += res.connected_seeds;
                _connected_leeches += res.connected_leeches;

                if (trkr->server_is_down() || res.connected_leeches + res.connected_seeds == 0) {
                    continue;
                }

                announce_result ret = trkr->announce(this, announce_started);
                _peer_mgr.insert(ret.peers.get());
            }

            _status = status::idle;
            return true;

        }


        std::array<char, 20> info_hash_bytes() const {
            std::array<char, 20> ret;
            std::copy_n(_meta->info_hash_bytes().begin(), 20, ret.begin());
            return ret;
        }

        std::array<char, 20> get_peer_id() {
            return _peer_id;
        }

        int64_t bytes_downloaded() const {
            return 0;
        }

        int64_t bytes_left() const {
            return 0;
        }

        int64_t bytes_uploaded() const {
            return 0;
        }

        uint32_t client_key() const {
            return 0;
        }

        int16_t listening_port() const {
            return 0;
        }

        int32_t completed_downloads() {
            return _completed_downloads;
        }

        int32_t connected_seeds() {
            return _connected_seeds;
        }

        int32_t connected_leeches() {
            return _connected_leeches;
        }

    private:
        float upload_rate() const {

        }

        float download_rate() const {

        }

        float connection_saturation() const {

        }

        std::int32_t _completed_downloads{0};
        std::int32_t _connected_seeds{0};
        std::int32_t _connected_leeches{0};

        session_settings _settings;
        std::array<char, 20> _peer_id;
        std::map<std::string, std::shared_ptr<tracker>> _tracker_map;
        // std::set<peer_endpoint> _peer_endpoints;
        peer_manager _peer_mgr;

        std::shared_ptr<torrent> _meta{nullptr};
        status _status{status::unknown};
        state _state{state::running};

    };

}
