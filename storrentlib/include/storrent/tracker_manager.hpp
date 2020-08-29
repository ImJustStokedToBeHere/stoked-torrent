#pragma once
#include "storrent/info_hash.hpp"
#include "storrent/os_inc.hpp"
#include "storrent/tracker_request.hpp"
// #include "storrent/uri.hpp"

#include <boost/asio.hpp>
#include <stdexcept>
#include <memory>
#include <thread>

// #include "storrent/torrent.hpp"
// #include "storrent/remote_tracker.hpp"
// #include <string>
// #include <thread>

namespace storrent
{
    using boost::asio::ip::tcp;
    using boost::asio::ip::udp;

    struct torrent_session;
    struct tracker_manager;

    struct scrape_result
    {
    };

    struct announce_result
    {
    };

    // struct tracker
    //{
    // public:
    //    template <typename T>
    //    tracker(T&& tracker_connection) : impl{new tracker_impl<T>(std::forward<T>(tracker_connection))}
    //    {
    //    }

    //    std::future<scrape_result> scrape(const std::vector<info_hash>& hashes);
    //    std::future<announce_result> announce(const info_hash& hash, announce_event event);

    // private:
    //    struct tracker_interface
    //    {
    //        std::string url;
    //        virtual ~tracker_interface() {}
    //    };

    //

    //    template <typename T>
    //    struct tracker_impl : public tracker_interface
    //    {
    //        tracker_impl(T&& obj) : interface_obj(std::forward<T>(obj)) {}

    //        virtual ~tracker_impl() {}

    //    private:
    //        T interface_obj;
    //    };

    //    std::unique_ptr<tracker_interface> impl;
    //};

    struct tracker_manager : std::enable_shared_from_this<tracker_manager>
    {
        tracker_manager(std::shared_ptr<torrent_session> hsession) : io_ctx{1}, hostname_resolved_count{0} {}

        ~tracker_manager() {}

        void resolve_trackers();

        void start_io()
        {
            io_loop_guard = std::make_unique<work_guard_type>(this->io_ctx.get_executor());
            std::thread io_runner([&] { this->io_ctx.run(); });
        }

        // stop the asio loop gracefully, waiting for 'real' io tasks to complete
        void stop_after_io()
        {
            io_loop_guard.reset();
            // close_all_tracker_sockets();
        }

        // stops the io loop as soon as possible, discarding scheduled tasks
        void stop_io()
        {
            this->io_ctx.stop();
            io_loop_guard.reset();
        }

        void reset_io()
        {
            this->io_ctx.reset();
            io_loop_guard = std::make_unique<work_guard_type>(this->io_ctx.get_executor());
        }

        std::future<bool> is_ready();

    private:
        using work_guard_type = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;

        struct tracker
        {
            static constexpr auto DEFAULT_REQ_TIMEOUT_SECONDS = 15;

            tracker(std::string announce_url, std::shared_ptr<tracker_manager> tracker_manager)
                : url{std::move(announce_url)},
                  mgr{std::move(tracker_manager)},
                  req_timeout{tracker_manager->io_ctx},
                  deadline{tracker_manager->io_ctx}
            {
            }

            virtual std::future<scrape_result> scrape(const std::vector<info_hash>& hashes) = 0;
            virtual std::future<announce_result> announce(const info_hash& hash, announce_event event) = 0;
            void set_request_timeout_expiry(size_t seconds = DEFAULT_REQ_TIMEOUT_SECONDS);
            void async_wait();

        protected:
            std::string url;
            std::shared_ptr<tracker_manager> mgr;
            boost::asio::steady_timer req_timeout;
            boost::asio::steady_timer deadline;

            void check_timeout();
        };

        // static constexpr size_t s = sizeof(tracker);


        struct unconnected_tracker : public tracker
        {
            unconnected_tracker(std::string announce_url,
                                std::shared_ptr<tracker_manager> tracker_manager,
                                boost::asio::io_context& io_ctx);

            unconnected_tracker(unconnected_tracker&&) = default;
            unconnected_tracker(const unconnected_tracker&) = default;

            void handle_udp_hostname_resolved(const boost::system::error_code& error,
                                              udp::resolver::results_type results);

            void handle_tcp_hostname_resolved(const boost::system::error_code& error,
                                              tcp::resolver::results_type results);

            void handle_udp_socket_connect(const boost::system::error_code& err,
                                           const udp::endpoint& endpoint);
            void handle_tcp_socket_connect(const boost::system::error_code& err, const tcp::endpoint& endpoint);

            virtual std::future<scrape_result> scrape(const std::vector<info_hash>& hashes) override;
            virtual std::future<announce_result> announce(const info_hash& hash, announce_event event) override;
        private:
            udp::socket udp_sock;
            tcp::socket tcp_sock;
        };

        struct udp_tracker : public tracker
        {
            udp_tracker(std::string announce_url,
                        std::shared_ptr<tracker_manager> tracker_manager,
                        udp::socket&& sock,
                        udp::endpoint ep);
        private:
            std::size_t retry_count{0};
            udp::socket sock;
            udp::endpoint endpoint;

        };

        struct tcp_tracker : public tracker
        {
            tcp_tracker(std::string announce_url, std::shared_ptr<tracker_manager> tracker_manager)
                : tracker(std::move(announce_url), tracker_manager)
            {
            }
        };

        std::map<std::string, std::shared_ptr<tracker>> trackers;
        std::unique_ptr<work_guard_type> io_loop_guard;
        std::shared_ptr<torrent_session> sesh;
        boost::asio::io_context io_ctx;
        std::promise<bool> is_ready_promise;
        std::atomic<size_t> hostname_resolved_count;

        void set_is_ready();
    };

    // constexpr auto s = sizeof(tracker_manager);

} // namespace storrent