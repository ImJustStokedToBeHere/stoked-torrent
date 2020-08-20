#pragma once
#include "storrent/torrent.hpp"
#include <boost/asio.hpp>
#include <string>

namespace storrent
{
    struct tracker_manager
    {
        tracker_manager(boost::asio::io_context* trackers_io_ctx, torrent_handle htor);

        void start(handle_peers_func get_peers_callback);
        void stop();

    private:
        void handle_udp_resolve(const boost::system::error_code& err,
                                boost::asio::ip::udp::resolver::results_type results,
                                const std::string& announce_url);
      
        void handle_tcp_resolve(const boost::system::error_code& err,
                                boost::asio::ip::tcp::resolver::results_type results);

        int handle_peers_list(const std::vector<std::string>& peers);
        
        bool m_stopped;
        boost::asio::steady_timer m_heartbeat;
        torrent_handle m_tor;
        boost::asio::io_context* m_io_ctx;
    };

} // namespace storrent
