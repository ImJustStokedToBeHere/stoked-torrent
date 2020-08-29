#include "storrent/tracker_manager.hpp"
#include "storrent/torrent.hpp"
#include "storrent/torrent_session.hpp"
#include "storrent/error_handling.hpp"
#include "storrent/uri.hpp"
#include <boost/bind/bind.hpp>
#include <boost/function.hpp>

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

namespace storrent
{
    /*
     * storrent::tracker_manager ===============================================
     */

    // this should happen relatively quickly and it returns when
    void tracker_manager::resolve_trackers()
    {
        auto& tor = sesh->tor();
        auto announces = std::vector<std::string>(tor.announce_list());
        if (auto found = std::find(announces.begin(), announces.end(), tor.announce()); found == announces.end())
        {
            // the announce URL is not in the list so lets add it to the front of the list
            announces.insert(announces.begin(), tor.announce());
        }
        // remove any duplicates from the list
        announces.erase(std::unique(announces.begin(), announces.end()), announces.end());

        udp::resolver udp_resolver(this->io_ctx);
        tcp::resolver tcp_resolver(this->io_ctx);

        for (const auto& url_str : announces)
        {
            auto parsed_url = std::move(uri::parse(url_str));

            auto unconnected = std::make_shared<unconnected_tracker>(url_str, shared_from_this());

            unconnected->set_request_timeout_expiry();
            if (parsed_url.protocol == "udp")
            {
                udp_resolver.async_resolve(parsed_url.host,
                                           parsed_url.port,
                                           boost::bind(&unconnected_tracker::handle_udp_hostname_resolved,
                                                       unconnected.get(),
                                                       boost::asio::placeholders::error,
                                                       boost::asio::placeholders::results));
            }
            else if (parsed_url.protocol == "http" || parsed_url.protocol == "https")
            {
                tcp_resolver.async_resolve(parsed_url.host,
                                           parsed_url.port,
                                           boost::bind(&unconnected_tracker::handle_tcp_hostname_resolved,
                                                       unconnected.get(),
                                                       boost::asio::placeholders::error,
                                                       boost::asio::placeholders::results));
            }
            else
            {
            }

            unconnected.set_
        }
    }

    std::future<bool> tracker_manager::is_ready() { return this->is_ready_promise.get_future(); }

    void tracker_manager::set_is_ready()
    {
        if ((this->hostname_resolved_count++) == this->sesh->tor().announce_list().size())
            this->is_ready_promise.set_value(true);
    }

    /*
    * =========================================================================
    */

    /*
     * storrent::tracker =======================================================
     */

    void tracker_manager::tracker::set_request_timeout_expiry(size_t seconds)
    {
        req_timeout.expires_after(std::chrono::seconds(seconds));
    }

    void tracker_manager::tracker::async_wait() 
    {
        this->req_timeout.async_wait(boost::bind(&tracker::check_timeout, this));
    }

    void tracker_manager::tracker::check_timeout()
    {

    }

    /*
     * storrent::unconnected_tracker ===========================================
     */

    tracker_manager::unconnected_tracker::unconnected_tracker(std::string announce_url,
                                                                     std::shared_ptr<tracker_manager> tracker_manager,
                                                                     boost::asio::io_context& io_ctx)
        : tracker(std::move(announce_url), tracker_manager), udp_sock(io_ctx), tcp_sock(io_ctx)
    {
    }

    void tracker_manager::unconnected_tracker::handle_udp_hostname_resolved(const boost::system::error_code& error,
                                                                            udp::resolver::results_type results)
    {
        if (!error)
        {
            if (results.begin() != results.end())
            {
                // udp is connectionless, so this doesn't actually do any connecting, it finds a reachable endpoint and then calls the completion handler
                set_request_timeout_expiry();
                boost::asio::async_connect(udp_sock,
                                           results,
                                           boost::bind(&unconnected_tracker::handle_udp_socket_connect,
                                                       this,
                                                       boost::asio::placeholders::error,
                                                       boost::asio::placeholders::endpoint));

                // this->udp_sock->async_connect()
            }
            else
            {
                // there were no endpoints from the resolve call

            }
        }
        else
        {
            // there was an error during resolution
        }
        //    if (endpoints.begin() != endpoints.end())
        //    {
        //        this->sock_state = tracker_sock_state::resolved;
        //        this->deadline.expires_after(std::chrono::seconds(CONNECT_DEADLINE_SECONDS));

        //        boost::asio::async_connect(sock,
        //                                   endpoints,
        //                                   boost::bind(&udp_remote_tracker::handle_socket_connect,
        //                                               this,
        //                                               boost::asio::placeholders::error,
        //                                               boost::asio::placeholders::endpoint));
        //    }
        //    else
        //    {
        //        // there were endpoints passed to this, we shouldn't have gotten this far
        //        stop_socket();
        //    }
    }

    void tracker_manager::unconnected_tracker::handle_udp_socket_connect(const boost::system::error_code& err,
                                                                         const udp::endpoint& endpoint)
    {
        if (!err)
        {
            set_request_timeout_expiry();

            // boost::asio::post(boost::bind(&unconnected_tracker::transform_to_udp_tracker, this));
        }
        else
        {
        
        }
    }

    void tracker_manager::unconnected_tracker::handle_tcp_socket_connect(const boost::system::error_code& err,
                                                                         const tcp::endpoint& endpoint)
    {
    }

    std::future<scrape_result> tracker_manager::unconnected_tracker::scrape(const std::vector<info_hash>& hashes)
    {
        throw not_implemented("tracker_manager::unconnected_tracker::scrape", __FILE__, __LINE__);
    }

    std::future<announce_result> tracker_manager::unconnected_tracker::announce(const info_hash& hash,
                                                                                announce_event event)
    {
        throw not_implemented("tracker_manager::unconnected_tracker::announce", __FILE__, __LINE__);
    }

    void tracker_manager::unconnected_tracker::handle_tcp_hostname_resolved(const boost::system::error_code& error,
                                                                            tcp::resolver::results_type results)
    {
        // hostname_resolved_count++;
        // check_and_set_is_ready();
        // set_is_ready();
    }

    /*
     * =========================================================================
     */

    /*
     * storrent::udp_tracker ===================================================
     */

    tracker_manager::udp_tracker::udp_tracker(std::string announce_url,
                                              std::shared_ptr<tracker_manager> tracker_manager,
                                              udp::socket&& sock,
                                              udp::endpoint ep)
        : tracker(std::move(announce_url), tracker_manager), sock{std::move(sock)}, endpoint{std::move(ep)}
    {
    }

    /*
     * =========================================================================
     */

    

    


    //tracker_manager::udp_tracker::udp_tracker(udp::socket&& sck, udp::endpoint ep, std::shared_ptr<tracker_manager> tracker_mgr) 
    //    : tracker(), sock{std::forward(sck)}, endpoint{std::move(ep)}
    //{

    //}

    

    // std::future<scrape_result> tracker_manager::tracker::scrape(const std::vector<info_hash>& hashes)
    //{
    //    return std::future<scrape_result>();
    //}

    // std::future<announce_result> tracker_manager::tracker::announce(const info_hash& hash, announce_event event)
    //{
    //    return std::future<announce_result>();
    //}

    // tracker_manager::tracker_manager(std::shared_ptr<torrent_session> hsession)
    //    : io_ctx{1}, heartbeat{io_ctx}, stopped{false}, sesh_ptr{hsession}, work_runner{io_ctx}
    //{
    //}

    // tracker_manager::~tracker_manager()
    //{
    //    if (!this->stopped)
    //        stop();
    //}

    // void tracker_manager::start_socket(std::function<int(const std::vector<std::string>&)> get_peers_callback)
    //{
    //    this->on_get_peers = get_peers_callback;

    //    // check to see if tthe announce is in the announce list, if not we'll add it to the top of the list
    //    // we add it to the front of the list because we're hoping that since this is 'THE Announce' it will have
    //    // the best peers and seeds so we can hopefully accumulate enough of them quickly rather than iterating
    //    // the entire announce list and then having to report to all of the trackers
    //    const torrent& tor = this->sesh().tor();
    //    auto announces = tor.announce_list();
    //    if (auto found = std::find(announces.begin(), announces.end(), tor.announce());
    //        found == announces.end())
    //    {
    //        auto itr = announces.begin();
    //        announces.insert(itr, tor.announce());
    //    }
    //    // remove any duplicates before we start processing the list, this may be a waste of time though
    //    std::sort(announces.begin(), announces.end());
    //    announces.erase(std::unique(announces.begin(), announces.end()), announces.end());

    //    // this->io_ctx.run();
    //    // now lets resolve some tracker hosts and make trackers out of them on successful resolution

    //    // we're making these locals because hopefully this is only called once per session for a single torrent
    //    boost::asio::ip::udp::resolver udp_resolver(this->io_ctx);
    //    boost::asio::ip::tcp::resolver tcp_resolver(this->io_ctx);

    //    for (auto& announce : announces)
    //    {
    //        // if it's a udp url then we'll resolve to udp otherwise...
    //        auto parsed_uri = uri::parse(announce);

    //        if (parsed_uri.protocol.starts_with("udp"))
    //        {
    //            // I don't think boost::bind is the recommended way of passing the handler
    //            // callback, it should be a lambda but I think the syntax is crowded and confusing
    //            // boost::asio::ip::udp::resolver::query q(url);
    //            udp_resolver.async_resolve(parsed_uri.host,
    //                                       parsed_uri.port,
    //                                       boost::bind(&tracker_manager::handle_udp_resolve,
    //                                                   this,
    //                                                   boost::asio::placeholders::error,
    //                                                   boost::asio::placeholders::results));
    //        }
    //        else if (parsed_uri.protocol.starts_with("http"))
    //        {
    //            tcp_resolver.async_resolve(parsed_uri.host,
    //                                       parsed_uri.port,
    //                                       boost::bind(&tracker_manager::handle_tcp_resolve,
    //                                                   this,
    //                                                   boost::asio::placeholders::error,
    //                                                   boost::asio::placeholders::results));
    //        }
    //    }

    //    // this->work_runner = boost::asio::io_context::work(this->io_ctx);
    //    this->io_ctx_thread = std::thread([this]() { this->io_ctx.run(); });
    //}

    // void tracker_manager::stop() { this->io_ctx.stop(); }

    //// on successful resolution we can now make tracker_manager for them and start fetching some peers,
    // void tracker_manager::handle_udp_resolve(const boost::system::error_code& err,
    //                                         boost::asio::ip::udp::resolver::results_type results)
    //{
    //    udp_remote_tracker tracker(this->io_ctx, shared_from_this());

    //    if (!err)
    //    {
    //        tracker.start_socket(results, [&](const auto& peers) -> int { return this->handle_peers_list(peers); });
    //    }
    //    else
    //    {
    //        //// there was an error trying to resolve the udp host so now lets
    //        //std::cout << "error: " << err.message() << std::endl;
    //    }
    //}

    // int tracker_manager::handle_peers_list(const std::vector<std::string>& peers) { return this->on_get_peers(peers);
    // }



    

    // void tracker_manager::handle_tcp_resolve(const boost::system::error_code& err,
    //                                         boost::asio::ip::tcp::resolver::results_type results)
    //{
    //    /*if (!err)
    //    {
    //        std::cout << "success !" << std::endl;
    //    }
    //    else
    //    {
    //        std::cout << "error: " << err.message() << std::endl;
    //    }*/
    //}
} // namespace storrent