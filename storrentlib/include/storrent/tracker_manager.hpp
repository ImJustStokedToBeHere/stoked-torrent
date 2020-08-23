#pragma once
#include "storrent/os_inc.hpp"
#include "storrent/torrent.hpp"
#include "storrent/remote_tracker.hpp"

#include <boost/asio.hpp>
//#include <tbb/concurrent_unordered_map.h>

#include <string>
#include <thread>

namespace storrent
{
    struct tracker_manager
    {
        tracker_manager(torrent_handle htor);

        void start(std::function<int(const std::vector<std::string>&)> get_peers_callback);
        void stop();

    private:
        std::thread& get_thread() { return this->io_ctx_thread; }

        void handle_udp_resolve(const boost::system::error_code& err,
                                boost::asio::ip::udp::resolver::results_type results);
      
        void handle_tcp_resolve(const boost::system::error_code& err,
                                boost::asio::ip::tcp::resolver::results_type results);

        int handle_peers_list(const std::vector<std::string>& peers);

        torrent_handle htor;
        std::function<int(const std::vector<std::string>&)> on_get_peers;
        boost::asio::io_context io_ctx;
        boost::asio::steady_timer heartbeat;
        //tbb::concurrent_unordered_map<std::string, std::unique_ptr<remote_tracker>> tracker_map;
        std::thread io_ctx_thread;
        bool stopped;
    };
} // namespace storrent
