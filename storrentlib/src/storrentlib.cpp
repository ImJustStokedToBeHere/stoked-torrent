// storrentlib.cpp : Defines the functions for the static library.
#include "storrent/storrentlib.hpp"
#include "storrent/pieces.hpp"
#include "storrent/torrent.hpp"
#include "storrent/tracker_manager.hpp"
#include "storrent/utils.hpp"
#include <boost/asio.hpp>

namespace storrent
{
    namespace
    {
        std::vector<std::string> peers_list;

        struct downloader
        {
            void go(const std::vector<std::string>& peers) {}
        };
    } // namespace

    int handle_peers(const std::vector<std::string>& peers)
    {
        downloader dl;
        dl.go(peers);
        return peers.size();
    }

    session_id download_file(const std::string& filename)
    {
        // make sure our file exists
        if (!utils::file_exists(filename))
            return INVALID_SESSION_ID;
        // first lets load our torrent file
        auto tor = torrent::load_torrent_file(filename);
        boost::asio::io_context io_ctx(1);
        tracker_manager trackers(&io_ctx, tor);
        trackers.start(&handle_peers);

        return INVALID_SESSION_ID;
    }

    session_id get_download_progress(session_id download_id, download_progress& progress) { return INVALID_SESSION_ID; }

    // typedef int (*handle_peers_from_tracker)(torrent& tor, const std::vector<std::string>& peer_list);

    // int handle_peers_list(torrent& tor, const std::vector<std::string>& peers)
    //{
    //    for (auto& p : peers)
    //    {
    //    }
    //}

    // void start(torrent& tor, handle_peers_from_tracker peers_callback)
    //{
    //    // check  to see if there is a single announce
    //
    //}
} // namespace storrent
