#pragma once
#include "storrent/os_inc.hpp"
#include "storrent/torrent.hpp"
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <functional>
#include <memory>
#include <utility>

namespace storrent
{
//    enum class tracker_sock_state
//    {
//        none = 0,
//        resolved,
//        connected,
//        timed_out,
//        stopped,
//        ok
//    };
//
//#define BYTES_ADDR(x) (static_cast<const char*>(static_cast<const void*>(&x)))
//#define BYTES_ADDR_MUT(x) (static_cast<char*>(static_cast<void*>(&x)))
//#define BYTES_ADDR_OFFSET(x, n) (static_cast<const char*>(static_cast<const void*>(&x)) + n)
//#define BYTES_ADDR_OFFSET_MUT(x, n) (static_cast<char*>(static_cast<void*>(&x)) + n)
//
//    // typedef std::function<int(const std::vector<std::string>&)> handle_peers_func;
//    struct tracker_manager;
//    struct remote_tracker
//    {
//        /*enum class announce_state
//        {
//            none,
//            requesting_peers,
//            completed,
//            started,
//            stopped
//        };*/
//
//        remote_tracker(std::shared_ptr<tracker_manager> trackers_ptr);
//        virtual ~remote_tracker() {}
//
//        virtual tracker_sock_state get_ip_state() const { return this->sock_state; }
//        virtual void set_ip_state(tracker_sock_state st) { this->sock_state = st; }
//    protected:

//        static constexpr auto WRITE_DEADLINE_SECONDS = 15;
//        static constexpr auto READ_DEADLINE_SECONDS = 15;
//
//        static constexpr std::int64_t PROTOCOL_ID = 0x41727101980;
//
//        static constexpr std::int32_t ACTION_CONNECT = 0x0;
//        static constexpr std::int32_t ACTION_ANNOUNCE = 0x1;
//        static constexpr std::int32_t ACTION_SCRAPE = 0x2;
//        static constexpr std::int32_t ACTION_ERROR = 0x3;
//
//        static constexpr std::int32_t ANNOUNCE_NONE = 0x0;
//        static constexpr std::int32_t ANNOUNCE_COMPLETED = 0x1;
//        static constexpr std::int32_t ANNOUNCE_STARTED = 0x2;
//        static constexpr std::int32_t ANNOUNCE_STOPPED = 0x3;
//
//        static std::int64_t make_big_tx_id();
//        static std::int32_t make_small_tx_id();
//
//        std::int32_t get_transaction_id() const { return this->tx_id; }
//        std::int64_t get_connection_id() const { return this->connection_id; }
//        
//
//        std::int32_t tx_id{0};
//        std::int64_t connection_id{0};
//        std::shared_ptr<tracker_manager> trackers_ptr{nullptr};
//        tracker_sock_state sock_state{tracker_sock_state::none};
//        std::int32_t announce_event{0};
//        std::function<void(const boost::system::error_code& err, std::size_t bytes_transferred)> announce_handler;
//    };
//
//    struct udp_remote_tracker : public remote_tracker
//    {
//        udp_remote_tracker(boost::asio::io_context& io_ctx, std::shared_ptr<tracker_manager> trackers_ptr);
//
//        virtual ~udp_remote_tracker() override { this->sock.close(); }
//
//        void start_socket(boost::asio::ip::udp::resolver::results_type endpoints);
//
//        // void announce(std::int32_t announce_event, )
//        void announce(std::int32_t announce_event,
//                      std::function<void(const boost::system::error_code& err, std::size_t bytes_transferred)>
//                          announce_response_handler);
//
//        void scrape(std::function<void(const boost::system::error_code& err, std::size_t bytes_transferred)>
//                        announce_response_handler);
//        
//        void stop_socket();
//    private:
//        void do_connect_then_announce();
//        void do_announce();
//
//        bool tracker_connected() const { this->connection_id != 0; }
//        void start_socket_connect(boost::asio::ip::udp::resolver::results_type endpoints);
//        void handle_retry_socket_connect(const boost::system::error_code& err);
//        void handle_socket_connect(const boost::system::error_code& err, const boost::asio::ip::udp::endpoint& endpoint);
//        void start_write_connect();
//        void handle_send_connect_req(const boost::system::error_code& err, std::size_t bytes_transferred);
//        void handle_recv_connect_response(const boost::system::error_code& err, std::size_t bytes_transferred);
//        void reset_connection() { this->connection_id = 0; }
//        void reset_transaction() { this->tx_id = make_small_tx_id(); }
//        void setup_connect_packet();
//        void setup_announce_packet(std::int32_t announce_event);
//        
//        void check_timeout();
//
//        const std::string& get_peer_id() const { return {}; }
//
//        template <typename InpType>
//        void write_reverse_to_buffer(const InpType& data, size_t data_size, char* buffer, size_t buf_offset)
//        {
//            // reserve temp mem
//            auto temp = std::make_unique<char[]>(data_size);
//            // copy the input to the temp
//            std::copy(BYTES_ADDR(data), BYTES_ADDR_OFFSET(data, data_size), temp.get());
//            // reverse the temp
//            utils::reverse_bytes(temp.get(), data_size);
//            // write the temp to the buffer
//            std::copy(temp.get(), temp.get() + data_size, buffer + buf_offset);
//        }
//
//        template <typename OutpType>
//        void read_reverse_from_buffer(OutpType& out_data, size_t data_size, char* buffer, size_t buf_offset)
//        {
//            // reserve temp mem
//            auto temp = std::make_unique<char[]>(data_size);
//            // copy from the buffer to the temp
//            std::copy(BYTES_ADDR_OFFSET(buffer, buf_offset),
//                      BYTES_ADDR_OFFSET(buffer, buf_offset + data_size),
//                      BYTES_ADDR_MUT(temp));
//            // reverse the temp data
//            utils::reverse_bytes(temp.get(), data_size);
//            // copy the now correct data to the output
//            std::copy(temp.get(), temp.get() + data_size, BYTES_ADDR_MUT(out_data));
//        }
//
//        
//
//        boost::asio::io_context* m_io_ctx{nullptr};
//        boost::asio::ip::udp::socket sock;
//        boost::asio::steady_timer deadline;
//        boost::asio::ip::udp::endpoint endpoint;
//
//        std::function<int(const std::vector<std::string>&)> m_get_peers_callback;
//        std::vector<char> in_data;
//        std::unique_ptr<char[]> out_data;
//        bool stopped;
//    };
//
//    constexpr auto s = sizeof(boost::asio::ip::udp::socket);
} // namespace storrent
