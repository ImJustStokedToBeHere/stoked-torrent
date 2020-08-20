#include "storrent/tracker_manager.hpp"
#include "storrent/remote_tracker.hpp"
#include <algorithm>
#include <boost/bind/bind.hpp>
#include <boost/function.hpp>

namespace storrent
{
    tracker_manager::tracker_manager(boost::asio::io_context* io_ctx, torrent_handle htor)
        : m_io_ctx{io_ctx}, m_heartbeat{*io_ctx}, m_stopped{false}, m_tor{htor}
    {
    }

    void tracker_manager::start(handle_peers_func get_peers_callback)
    {
        // check to see if tthe announce is in the announce list, if not we'll add it to the top of the list
        // we add it to the front of the list because we're hoping that since this is 'THE Announce' it will have
        // the best peers and seeds so we can hopefully accumulate enough of them quickly rather than iterating
        // the entire announce list and then having to report to all of the trackers
        auto announces = m_tor->announce_list();
        if (auto found = std::find(announces.begin(), announces.end(), m_tor->announce()); found == announces.end())
        {
            auto itr = announces.begin();
            announces.insert(itr, m_tor->announce());
        }

        // now lets resolve some tracker hosts and make trackers out of them on successful resolution

        // we're making these locals because hopefully this is only called once per session for a single torrent
        boost::asio::ip::udp::resolver udp_resolver(*m_io_ctx);
        boost::asio::ip::tcp::resolver tcp_resolver(*m_io_ctx);

        for (auto& url : announces)
        {
            // if it's a udp url then we'll resolve to udp otherwise...
            std::string host;
            std::string port;

            if (url.starts_with("udp"))
            {
                // I don't think boost::bind is the recommended way of passing the handler
                // callback, it should be a lambda but I think the syntax is crowded and confusing
                udp_resolver.async_resolve(host,
                                           port,
                                           boost::bind(&tracker_manager::handle_udp_resolve,
                                                       this,
                                                       boost::asio::placeholders::error,
                                                       boost::asio::placeholders::iterator,
                                                       url));
            }
            else if (url.starts_with("http"))
            {
                tcp_resolver.async_resolve(host,
                                           port,
                                           boost::bind(&tracker_manager::handle_tcp_resolve,
                                                       this,
                                                       boost::asio::placeholders::error,
                                                       boost::asio::placeholders::iterator));
            }
        }
    }

    void tracker_manager::stop() {}

    // on successful resolution we can now make tracker_manager for them and start fetching some peers,
    void tracker_manager::handle_udp_resolve(const boost::system::error_code& err,
                                             boost::asio::ip::udp::resolver::results_type results,
                                             const std::string& announce_url)
    {
        udp_remote_tracker tracker(m_io_ctx, m_tor);
        tracker.start(results, [&](const auto& peers) -> int { return this->handle_peers_list(peers); });
    }

    int tracker_manager::handle_peers_list(const std::vector<std::string>& peers) 
    {

    }

    void tracker_manager::handle_tcp_resolve(const boost::system::error_code& err,
                                             boost::asio::ip::tcp::resolver::results_type results)
    {
        if (!err)
        {
        }
        else
        {
        }
    }
} // namespace storrent