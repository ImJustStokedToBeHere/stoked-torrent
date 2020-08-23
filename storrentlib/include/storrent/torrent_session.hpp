#pragma once
#include "storrent/torrent.hpp"
#include "storrent/tracker_manager.hpp"
#include "storrent/utils.hpp"
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace storrent
{
    // typedef int session_id;
    struct torrent_session
    {
        inline static const info_hash INVALID_ID;

        torrent_session(const std::string& filename)
        {
            htor = torrent::load_torrent_file(filename);
            trackers = std::make_shared<tracker_manager>(htor);
        }

        torrent_session(const torrent_session&) noexcept = delete;
        torrent_session(torrent_session&& other) noexcept {}

        ~torrent_session() {}

        torrent_session& operator=(const torrent_session&) = delete;

        torrent_session& operator=(torrent_session&& other) noexcept { return *this; }

        inline static std::shared_ptr<torrent_session> make(const std::string& torrent_filename)
        {
            if (!utils::file_exists(torrent_filename))
                return nullptr;

            return std::make_shared<torrent_session>(torrent_filename);
        }

        inline static std::shared_ptr<torrent_session> resume(const std::string& torrent_filename) { return nullptr; }

        void start_upload() {}

        void start_download()
        {
            this->trackers->start(std::bind(&torrent_session::handle_peers, this, std::placeholders::_1));
        }

        void stop_upload() {}

        void stop_download() { this->trackers->stop(); }

        const info_hash& get_id() { return this->htor->info_hash_val(); }

    private:
        std::shared_ptr<tracker_manager> trackers{nullptr};
        torrent_handle htor{nullptr};

        int handle_peers(const std::vector<std::string>& peers) { return 0; }
    };
} // namespace storrent
