#pragma once
#include "storrent/info_hash.hpp"
#include "storrent/os_inc.hpp"
#include "storrent/tracker_request.hpp"
#include <boost/asio.hpp>
#include <future>
#include <memory>
#include <stdexcept>
#include <thread>
#include <vector>

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

    struct tracker_manager : std::enable_shared_from_this<tracker_manager>
    {
        tracker_manager(std::shared_ptr<torrent_session> hsession) 
            : sesh{hsession},
             hostname_resolved_count{0}, udp_resolver{this->io_ctx}, tcp_resolver{this->io_ctx}
        {}

        ~tracker_manager() {}

        void resolve_trackers();

        std::vector<std::shared_future<scrape_result>> get_scrape_results(const std::vector<info_hash>& hashes);

        void start_io();

        // stop the asio loop gracefully, waiting for 'real' io tasks to complete
        void stop_after_io();

        // stops the io loop as soon as possible, discarding scheduled tasks
        void stop_io();

        void reset_io();

        std::shared_future<bool> is_ready();

    private:
        using work_guard_type = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;

        struct tracker;
        struct udp_tracker;
        struct tcp_tracker;
        struct unconnected_tracker;

        std::map<std::string, std::shared_ptr<tracker>> trackers;
        std::unique_ptr<work_guard_type> io_loop_guard;
        std::shared_ptr<torrent_session> sesh;
        boost::asio::io_context io_ctx{1};
        std::promise<bool> is_ready_promise;
        std::atomic<size_t> hostname_resolved_count;
        std::thread io_runner;
        udp::resolver udp_resolver;
        tcp::resolver tcp_resolver;

        void update_is_ready();

        struct tracker
        {
            enum class proc_state
            {
                none,
                faulty,
                stopping,
                idling,
                sending_connect_req,
                receiving_connect_resp,
                sending_scrape_req,
                receiving_scrape_resp,
                sending_announce_req,
                recieving_announce_resp,
            };

            static constexpr auto DEFAULT_REQ_TIMEOUT_SECONDS = 15;
            static constexpr auto MAX_TIMED_OUT_RETRY_COUNT = 8;
            static constexpr auto RESOLUTION_TIMEOUT_SECONDS = 60;

            tracker(std::string announce_url, std::shared_ptr<tracker_manager> tracker_manager);
            virtual ~tracker() {}

            virtual std::shared_future<scrape_result> scrape(const std::vector<info_hash>& hashes) = 0;
            virtual std::shared_future<announce_result> announce(const info_hash& hash, announce_event event) = 0;

            void set_request_timeout_expiry(std::size_t seconds);
            void set_request_timeout_expiry();
            void async_wait();

            tracker::proc_state process_state() const { return this->state; }

            void set_process_state(tracker::proc_state state)
            {
                if (this->state != state)
                    this->state = state;
            }

            // bool is_stopped() const { return this->state == proc_state::none; }
            bool is_faulty() const { return this->state == proc_state::faulty; }
            bool is_idling() const { return this->state == proc_state::idling; }
            const std::string& host_url() const { return this->url; }
        protected:
            proc_state state;
            std::string url;
            std::shared_ptr<tracker_manager> mgr;
            boost::asio::steady_timer req_timeout;
            boost::asio::steady_timer deadline;
            std::uint8_t retry_count{0};

            void write_send_buffer(std::vector<char>& send, const tracker_request& req);
            virtual void write_field_to_buffer(std::vector<char>& send, size_t idx, const tracker_request& req) = 0;

            virtual void do_timeout() = 0;
        };

        struct unconnected_tracker : public tracker
        {
            unconnected_tracker(std::string announce_url,
                                std::shared_ptr<tracker_manager> tracker_manager/*,
                                boost::asio::io_context& io_ctx*/);

            unconnected_tracker(unconnected_tracker&&) = default;
            unconnected_tracker(const unconnected_tracker&) = default;

            void handle_udp_hostname_resolved(const boost::system::error_code& error,
                                              udp::resolver::results_type results);

            void handle_tcp_hostname_resolved(const boost::system::error_code& error,
                                              tcp::resolver::results_type results);

            virtual std::shared_future<scrape_result> scrape(const std::vector<info_hash>& hashes) override;
            virtual std::shared_future<announce_result> announce(const info_hash& hash, announce_event event) override;

        private:
            void shutdown_socket();
            bool is_udp{true};
            udp::socket udp_sock;
            tcp::socket tcp_sock;

            virtual void write_field_to_buffer(std::vector<char>& send,
                                               size_t idx, const tracker_request& req) override
            {
            }

            virtual void do_timeout() override;
            void handle_udp_socket_connect(const boost::system::error_code& err, const udp::endpoint& endpoint);
            void handle_tcp_socket_connect(const boost::system::error_code& err, const tcp::endpoint& endpoint);
            void make_udp_tracker();
        };

        struct udp_tracker : public tracker
        {
            udp_tracker(std::string announce_url,
                        std::shared_ptr<tracker_manager> tracker_manager,
                        udp::socket&& sock,
                        udp::endpoint ep);

            virtual std::shared_future<scrape_result> scrape(const std::vector<info_hash>& hashes) override;
            virtual std::shared_future<announce_result> announce(const info_hash& hash, announce_event event) override;

        private:
            udp::socket sock;
            udp::endpoint endpoint;
            std::promise<scrape_result> scrape_promise;
            std::promise<announce_result> announce_promise;
            std::promise<std::int64_t> connect_promise;
            std::vector<char> data;

            virtual void do_timeout() override;
            virtual void write_field_to_buffer(std::vector<char>& send,
                                               size_t idx,
                                               const tracker_request& req) override;

            template <typename InpType>
            void write_reverse_to_buffer(const InpType& data, size_t data_size, char* buffer, size_t buf_offset)
            {
                // reserve temp mem
                auto temp = std::make_unique<char[]>(data_size);
                // copy the input to the temp
                std::copy(BYTES_ADDR(data), BYTES_ADDR_OFFSET(data, data_size), temp.get());
                // reverse the temp
                utils::reverse_bytes(temp.get(), data_size);
                // write the temp to the buffer
                std::copy(temp.get(), temp.get() + data_size, buffer + buf_offset);
            }

            template <typename OutpType>
            void read_reverse_from_buffer(OutpType& out_data, size_t data_size, char* buffer, size_t buf_offset)
            {
                // reserve temp mem
                auto temp = std::make_unique<char[]>(data_size);
                // copy from the buffer to the temp
                std::copy(BYTES_ADDR_OFFSET(buffer, buf_offset),
                          BYTES_ADDR_OFFSET(buffer, buf_offset + data_size),
                          BYTES_ADDR_MUT(temp));
                // reverse the temp data
                utils::reverse_bytes(temp.get(), data_size);
                // copy the now correct data to the output
                std::copy(temp.get(), temp.get() + data_size, BYTES_ADDR_MUT(out_data));
            }

            void connect();
            void handle_recv_connect(const boost::system::error_code& error, std::size_t bytes_transferred); 
            std::future<std::int64_t> get_connection_id();
        };

        struct tcp_tracker : public tracker
        {
            tcp_tracker(std::string announce_url, std::shared_ptr<tracker_manager> tracker_manager)
                : tracker(std::move(announce_url), tracker_manager)
            {
            }
        };
    };

    // constexpr auto s = sizeof(tracker_manager);

} // namespace storrent