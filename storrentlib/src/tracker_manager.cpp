#include "storrent/tracker_manager.hpp"
#include "storrent/error_handling.hpp"
#include "storrent/torrent.hpp"
#include "storrent/torrent_session.hpp"
#include "storrent/uri.hpp"
#include "storrent/debug.hpp"
#include <algorithm>
#include <boost/bind/bind.hpp>
#include <boost/function.hpp>
#include <cmath>
#include <memory>
#include <string>
#include <vector>
#include <random>

namespace storrent
{
    /*
     * storrent::tracker_manager ===============================================
     */

    // this should happen relatively quickly and it returns as soon as we're done looping the announce urls
    void tracker_manager::resolve_trackers()
    {
        auto tor = this->sesh->tor();
        auto announces = std::vector<std::string>(tor->announce_list());
        if (auto found = std::find(announces.begin(), announces.end(), tor->announce()); found == announces.end())
        {
            // the announce URL is not in the list so lets add it to the front of the list
            announces.insert(announces.begin(), tor->announce());
        }
        // remove any duplicates from the list
        announces.erase(std::unique(announces.begin(), announces.end()), announces.end());

        /*udp::resolver udp_resolver(this->io_ctx);
        tcp::resolver tcp_resolver(this->io_ctx);*/

        for (const auto& url_str : announces)
        {
            auto parsed_url = std::move(uri::parse(url_str));

            auto unconnected = std::make_shared<unconnected_tracker>(url_str, shared_from_this());
            // trackers.insert(url_str, unconnected);
            trackers.insert(std::make_pair(url_str, unconnected));

            unconnected->set_request_timeout_expiry(tracker::RESOLUTION_TIMEOUT_SECONDS);
            // unconnected->set_process_state(tracker::proc_state::resolving);
            if (parsed_url.protocol == "udp")
            {
                TRACE_OUT("start resolving udp host: " << url_str);
                udp_resolver.async_resolve(parsed_url.host,
                                           parsed_url.port,
                                           boost::bind(&unconnected_tracker::handle_udp_hostname_resolved,
                                                       unconnected.get(),
                                                       boost::asio::placeholders::error,
                                                       boost::asio::placeholders::results));
            }
            else if (parsed_url.protocol == "http" || parsed_url.protocol == "https")
            {

                TRACE_OUT("start resolving tcp host: " << url_str);

                tcp_resolver.async_resolve(parsed_url.host,
                                           parsed_url.port,
                                           boost::bind(&unconnected_tracker::handle_tcp_hostname_resolved,
                                                       unconnected.get(),
                                                       boost::asio::placeholders::error,
                                                       boost::asio::placeholders::results));
            }
            else
            {
                // this is a crap url

            }

            unconnected->async_wait();
        }
    }

    std::vector<std::shared_future<scrape_result>> tracker_manager::get_scrape_results(const std::vector<info_hash>& hashes)
    {
        // auto hashes = this->sesh->get_hashes();
        std::vector<std::shared_future<scrape_result>> results;
        
        for (auto& pair : this->trackers)
        {
            auto trkr = pair.second;
            if (trkr->is_idling())
            {
                TRACE_OUT("idling ready to scrape: " << trkr->host_url());
                results.emplace_back(trkr->scrape(hashes));

            }
        }

        return results;
    }

    void tracker_manager::start_io()
    {
        io_loop_guard = std::make_unique<work_guard_type>(this->io_ctx.get_executor());
        io_runner = std::thread([&] { this->io_ctx.run(); });
    }

    // stop the asio loop gracefully, waiting for 'real' io tasks to complete

    void tracker_manager::stop_after_io()
    {
        io_loop_guard.reset();
        // close_all_tracker_sockets();
    }

    // stops the io loop as soon as possible, discarding scheduled tasks

    void tracker_manager::stop_io()
    {
        this->io_ctx.stop();
        io_loop_guard.reset();
    }

    void tracker_manager::reset_io()
    {
        this->io_ctx.reset();
        io_loop_guard = std::make_unique<work_guard_type>(this->io_ctx.get_executor());
    }

    std::shared_future<bool> tracker_manager::is_ready()
    {
        resolve_trackers();
        this->start_io();
        return this->is_ready_promise.get_future().share();
    }

    void tracker_manager::update_is_ready()
    {
        auto count = this->sesh->tor()->announce_list().size();
        auto resolved = (++this->hostname_resolved_count);
        if (resolved == count)
            this->is_ready_promise.set_value(true);
    }

    /*
     * =========================================================================
     */

    /*
     * storrent::tracker =======================================================
     */

    tracker_manager::tracker::tracker(std::string announce_url, std::shared_ptr<tracker_manager> tracker_manager)
        : url{std::move(announce_url)},
          mgr{std::move(tracker_manager)},
          req_timeout{mgr->io_ctx},
          deadline{mgr->io_ctx},
          state{proc_state::none}
    {
    }

    void tracker_manager::tracker::set_request_timeout_expiry(std::size_t seconds)
    {
        this->req_timeout.expires_after(std::chrono::seconds(seconds));
    }

    void tracker_manager::tracker::set_request_timeout_expiry()
    {
        // calc the timeout time
        size_t seconds = 15 * std::pow(2, retry_count);
        this->set_request_timeout_expiry(seconds);
    }

    void tracker_manager::tracker::async_wait()
    {
        this->req_timeout.async_wait(boost::bind(&tracker::do_timeout, this));
    }

    void tracker_manager::tracker::write_send_buffer(std::vector<char>& send, const tracker_request& req) 
    {
        for (size_t i = 0; i < req.field_count(); i++)
        {
            write_field_to_buffer(send, i, req);
        }
    }

    /*
     * storrent::unconnected_tracker ===========================================
     */

    tracker_manager::unconnected_tracker::unconnected_tracker(std::string announce_url,
                                                              std::shared_ptr<tracker_manager> tracker_manager/*,
                                                              boost::asio::io_context& io_ctx*/)
        : tracker(std::move(announce_url), tracker_manager), udp_sock(tracker_manager->io_ctx), tcp_sock(tracker_manager->io_ctx)
    {
    }

    void tracker_manager::unconnected_tracker::handle_udp_hostname_resolved(const boost::system::error_code& error,
                                                                            udp::resolver::results_type results)
    {
        if (!error)
        {
            if (results.begin() != results.end())
            {

                TRACE_OUT("handle udp hostname resolved. start socket connect: " << this->url);
                // udp is connectionless, so this doesn't actually do any connecting, it just pairs up the socket with
                // the endpoint then calls the completion handler
                set_request_timeout_expiry();
                boost::asio::async_connect(udp_sock,
                                           results,
                                           boost::bind(&unconnected_tracker::handle_udp_socket_connect,
                                                       this,
                                                       boost::asio::placeholders::error,
                                                       boost::asio::placeholders::endpoint));
            }
            else
            {
                TRACE_OUT("handle udp hostname unresolved. no endpoints resolved, shutting down socket: " << this->url);
                // no endpoints were resolved
                // shutdown the socket even though we probably dont need to
                shutdown_socket();
                // set the process state to faulty
                set_process_state(proc_state::faulty);
                // cancel the async_wait on the timeout timer
                this->req_timeout.cancel();

                std::cerr << "udp hostname resolution error: " << this->url
                          << " message: no endpoints resolved from hostname" << std::endl;

                this->mgr->update_is_ready();
            }
        }
        else
        {
            // there was an error during resolution
            std::cerr << "udp hostname resolution error: " << this->url << " message: " << error.message() << std::endl;
            this->mgr->update_is_ready();
        }
    }

    void tracker_manager::unconnected_tracker::handle_udp_socket_connect(const boost::system::error_code& err,
                                                                         const udp::endpoint& endpoint)
    {
        if (!err)
        {
            static const auto default_socket = udp::endpoint();
            if (!this->is_faulty() && this->udp_sock.remote_endpoint() != default_socket)
            {
                TRACE_OUT("handle udp socket connect. make tracker with addr: "
                          << this->url << " - " << endpoint.address() << ':' << endpoint.port());

                // we're not stopped and we're not faulty so we can go ahead and create a real tracket out of it
                boost::asio::post(this->mgr->io_ctx, boost::bind(&unconnected_tracker::make_udp_tracker, this));
            }
        }
        else
        {
            std::cerr << "udp endpoint connection error: " << this->url << " message: " << err.message() << std::endl;
            this->mgr->update_is_ready();
        }

        
    }

    void tracker_manager::unconnected_tracker::make_udp_tracker()
    {
        TRACE_OUT("make udp tracker: " << this->url << " : " << this->udp_sock.local_endpoint().address() << " -> "
                                       << this->udp_sock.remote_endpoint().address());

        //  get a copy of the unconnected tracker
        auto tracker_copy = std::static_pointer_cast<unconnected_tracker>(this->mgr->trackers[this->url]);
        // copy the remote endpoint
        auto ep = tracker_copy->udp_sock.remote_endpoint();
        // replace the unconnected trak with the udp_tracker
        this->mgr->trackers[this->url] = std::make_shared<udp_tracker>(tracker_copy->url,
                                                                       tracker_copy->mgr,
                                                                       std::move(tracker_copy->udp_sock),
                                                                       ep);

        this->mgr->update_is_ready();
    }

    void tracker_manager::unconnected_tracker::handle_tcp_socket_connect(const boost::system::error_code& err,
                                                                         const tcp::endpoint& endpoint)
    {
    }

    std::shared_future<scrape_result> tracker_manager::unconnected_tracker::scrape(const std::vector<info_hash>& hashes)
    {
        throw not_implemented("tracker_manager::unconnected_tracker::scrape", __FILE__, __LINE__);
    }

    std::shared_future<announce_result> tracker_manager::unconnected_tracker::announce(const info_hash& hash,
                                                                                announce_event event)
    {
        throw not_implemented("tracker_manager::unconnected_tracker::announce", __FILE__, __LINE__);
    }

    void tracker_manager::unconnected_tracker::do_timeout()
    {
        // if we get in here after we've already been stopped or faulted then we can
        // then immediately return
        if (this->state == proc_state::none || this->state == proc_state::faulty)
            return;

        if (this->req_timeout.expiry() <= boost::asio::steady_timer::clock_type::now())
        {
            shutdown_socket();
            set_process_state(proc_state::faulty);
            this->req_timeout.cancel();
        }

        this->req_timeout.async_wait(std::bind(&unconnected_tracker::do_timeout, this));
    }

    void tracker_manager::unconnected_tracker::shutdown_socket()
    {
        if (is_udp)
            this->udp_sock.close();
        else
            this->tcp_sock.close();
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

    namespace
    {
        static constexpr std::int64_t PROTOCOL_ID = 0x41727101980;
    
        static constexpr std::int32_t ACTION_CONNECT = 0x0;
        static constexpr std::int32_t ACTION_ANNOUNCE = 0x1;
        static constexpr std::int32_t ACTION_SCRAPE = 0x2;
        static constexpr std::int32_t ACTION_ERROR = 0x3;
    
        static constexpr std::int32_t ANNOUNCE_NONE = 0x0;
        static constexpr std::int32_t ANNOUNCE_COMPLETED = 0x1;
        static constexpr std::int32_t ANNOUNCE_STARTED = 0x2;
        static constexpr std::int32_t ANNOUNCE_STOPPED = 0x3;
    
        std::random_device rand_seed;
        std::mt19937_64 big_mersenne_engine = std::mt19937_64(rand_seed());
        std::mt19937 mersenne_engine = std::mt19937(rand_seed());

        std::int32_t make_tx_id() { return mersenne_engine(); }
        
    }


    tracker_manager::udp_tracker::udp_tracker(std::string announce_url,
                                              std::shared_ptr<tracker_manager> tracker_manager,
                                              udp::socket&& sock,
                                              udp::endpoint ep)
        : tracker(std::move(announce_url), tracker_manager), sock{std::move(sock)}, endpoint{std::move(ep)}
    {
        this->state = proc_state::idling;
    }

    std::shared_future<scrape_result> tracker_manager::udp_tracker::scrape(const std::vector<info_hash>& hashes)
    {
        //
        //if (!is_connected())
        //{

        //}





        this->async_wait();

        return std::shared_future<scrape_result>();
    }

    std::shared_future<announce_result> tracker_manager::udp_tracker::announce(const info_hash& hash, announce_event event)
    {
        return std::shared_future<announce_result>();
    }

    void tracker_manager::udp_tracker::do_timeout()
    {
        switch (this->process_state())
        {
            default:
                break;
        }
    }

    void tracker_manager::udp_tracker::write_field_to_buffer(std::vector<char>& send,
                                                             size_t idx,
                                                             const tracker_request& req)
    {
        // void write_reverse_to_buffer(const InpType& data, size_t data_size, char* buffer, size_t buf_offset)
        auto pair = req.field_data(idx);
        write_reverse_to_buffer(pair.first, pair.second, send.data(), req.field_offset(idx));
    }

    void tracker_manager::udp_tracker::connect() 
    { 
        this->set_process_state(proc_state::sending_connect_req);
        this->set_request_timeout_expiry();

        connect_request req(PROTOCOL_ID, ACTION_CONNECT, make_tx_id());
        data.resize(16, '\0');
        write_send_buffer(data, req);

        this->sock.async_send_to(boost::asio::buffer(data.data(), 16),
                                 endpoint,
                                 boost::bind(&udp_tracker::handle_recv_connect,
                                             this,
                                             boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred));
    }

    void tracker_manager::udp_tracker::handle_recv_connect(const boost::system::error_code& error,
                                                           std::size_t bytes_transferred)
    {


    }

    std::future<std::int64_t> tracker_manager::udp_tracker::get_connection_id() 
    { 
        connect();
        return connect_promise.get_future(); 
    }

    /*
     * =========================================================================
     */

} // namespace storrent