#pragma once
#include "storrent/torrent.hpp"
#include "storrent/tracker_manager.hpp"
#include "storrent/utils.hpp"
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <cstdint>

namespace storrent
{

    // typedef int session_id;
    struct torrent_session : std::enable_shared_from_this<torrent_session>
    {
        enum class state : std::uint16_t
        {
            stopped = 0x0,
            paused = 0x1,
            downloading = 0x2,
            seeding = 0x4,
            hashing = 0x8,
            stopping = 0x10,
            errored = 0x20,
            meta_srcing = 0x40
        };

        inline static const info_hash INVALID_ID;

        torrent_session(std::shared_ptr<torrent> tor_ptr, const std::string& source_file_directory_name)
            : src_dir_name{source_file_directory_name},
              cur_state{state::stopped},
              prev_state{state::stopped},
              tor_ptr{tor_ptr},
              trackers_ptr{std::make_shared<tracker_manager>(shared_from_this())}
        {
        }

        torrent_session(const torrent_session&) noexcept = delete;
        torrent_session(torrent_session&& other) noexcept = default;

        ~torrent_session() {}

        torrent_session& operator=(const torrent_session&) = delete;

        torrent_session& operator=(torrent_session&& other) noexcept { return *this; }

        static std::shared_ptr<torrent_session> make(const std::string& torrent_filename,
                                                     const std::string& source_file_directory_name);

        inline static std::shared_ptr<torrent_session> resume(const std::string& torrent_filename) { return nullptr; }

        void start_upload()
        {
            // here is where we need to figure out what pieces we have, if the file is complete then we have all of them
        }

        void start_download();

        void stop_upload() {}

        void stop_download() 
        { 
            //this->trackers().stop(); 
        }

        const info_hash& get_id() const { return this->tor().info_hash_val(); }

        const torrent& tor() const { return *(this->tor_ptr); }
        torrent& tor() { return *(this->tor_ptr); }

        tracker_manager& trackers() const { return *(this->trackers_ptr); }

        std::uint64_t get_downloaded_bytes_count() const { return 0; }
        std::uint64_t get_bytes_left_count() const { return 0; }
        std::uint64_t get_uploaded_bytes_count() const { return 0; }

        std::int32_t client_ip_addr() const { return 0; }
        std::int16_t client_port() const { return 0; }
        // we need to check then engine against available connections and connections in use by this and other 
        // session, how much we actually have downloaded, if we have enough peers that have all the pieces we need
        std::int32_t peers_wanted() const { return -1; }
    private:
        std::shared_ptr<tracker_manager> trackers_ptr;
        std::shared_ptr<torrent> tor_ptr;
        std::string src_dir_name;
        state cur_state;
        state prev_state;

        int handle_peers(const std::vector<std::string>& peers) { return 0; }
    };
} // namespace storrent