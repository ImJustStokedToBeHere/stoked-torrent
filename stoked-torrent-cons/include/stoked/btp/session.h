#pragma once
#include "tracker_manager.h"
#include "torrent.h"
#include "info_hash.h"
#include "utils.h"
#include "sys.h"
#include <boost/asio.hpp>
#include <string>
#include <array>
#include <set>
#include <memory>
#include <cstdint>

namespace stoked::btp
{
    using boost::asio::io_context;

    struct session_settings
    {
        std::string torrent_filename{};
        std::string download_folder{};
        bool create_subfolder{true};
        uint8_t priority{0};
        bool use_pex{true};
        bool use_dht{true};
        short upload_slot_count{0};
        short padding{0};
        std::array<char, 20> peer_id;
    };

    struct session
    {
        enum class state : char
        {
            running,
            errored
        };

        enum class status : char
        {
            unknown,
            idle,
            init,
            starting_trackers,
            gathering_peers,
            downloading,
            seeding
        };

        explicit session(const std::string& torrent_filename) noexcept
            : session(default_session_settings(torrent_filename))
        { }

        explicit session(const session_settings& settings) noexcept
            : m_settings{settings},
            m_tracker_mgr{m_ioctx}
        {
            std::string err;

            try
            {
                m_meta = torrent::load_torrent_file(m_settings.torrent_filename);
            }
            catch (const std::exception& except)
            {
                utils::report_error(utils::make_str("error loading torrent: ", except.what()));
            }

            try
            {
                // setup the peer id
                if (m_settings.peer_id.empty())
                {
                    std::string ver = "S_" + utils::module_info::get_file_version_info_value(utils::file_version_info_value::product_version) + "-";
                    utils::replace_substr(ver, ".", "", false, false);
                    std::copy(ver.begin(), ver.end(), m_peer_id.data());
                    utils::generate_random_byte_str(m_peer_id.data(), ver.length(), 20 - ver.length());
                }
                else
                {
                    std::copy(m_settings.peer_id.begin(), m_settings.peer_id.end(), m_peer_id.data());
                }
            }
            catch (const std::exception& except)
            {
                utils::report_error(utils::make_str("error setting peer id: ", except.what()));
            }            
        }

        session(session&&) noexcept = default;
        session(const session&) noexcept = delete;
        ~session() { }

        session& operator=(session&&) noexcept = default;
        session& operator=(const session&) noexcept = delete;

        static session_settings default_session_settings(std::string filename = std::string{})
        {
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

        void start()
        {
            init_trackers();
        }

    private:
        status m_status{status::unknown};
        std::shared_ptr<torrent> m_meta;
        session_settings m_settings;
        io_context m_ioctx;
        std::shared_ptr<std::thread> m_thread;
        tracker_manager m_tracker_mgr;
        std::array<char, 20> m_peer_id;


        void init_trackers()
        {
            m_status = status::init;
            std::set<std::string> tracker_urls;

            for (const auto& url : m_meta->announce_list())
            {
                // de-dup the announce list urls so we don't run multiples of the same tracker
                if (tracker_urls.find(url) == tracker_urls.end())
                {                   
                    tracker_urls.insert(url);
                    if (!m_tracker_mgr.add_endpoint(url))
                    {
                        utils::report_error("invalid tracker url: ", url);
                    }
                }
            }
        }
    };
}


