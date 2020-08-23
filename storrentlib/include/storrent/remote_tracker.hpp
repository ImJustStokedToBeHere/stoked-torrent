#pragma once
#include "storrent/os_inc.hpp"
#include "storrent/torrent.hpp"
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <functional>
#include <utility>

namespace storrent
{
    enum class tracker_state
    {
        none = 0,
        resolved,
        connected,
        timed_out,
        stopped,
        ok
    };

    // typedef std::function<int(const std::vector<std::string>&)> handle_peers_func;

    struct remote_tracker
    {
        static constexpr auto CONNECT_DEADLINE_SECONDS = 35;
        remote_tracker(torrent_handle htor) : m_htor{htor} {}
        virtual ~remote_tracker() {}

    protected:
        virtual std::shared_ptr<std::vector<char>> get_handshake_msg() const = 0;
        virtual std::size_t get_handshake_msg_len() const = 0;
        torrent_handle m_htor{nullptr};
    };

    struct udp_remote_tracker : public remote_tracker
    {
        udp_remote_tracker(boost::asio::io_context& io_ctx, torrent_handle htor)
            : remote_tracker(htor), sock(io_ctx), deadline(io_ctx), stopped{false}, state{tracker_state::none}
        {
        }

        virtual ~udp_remote_tracker() { this->sock.close(); }

        void start(boost::asio::ip::udp::resolver::results_type endpoints,
                   std::function<int(const std::vector<std::string>&)> get_peers_callback)
        {
            m_get_peers_callback = get_peers_callback;
            start_connect(endpoints);
            deadline.async_wait(boost::bind(&udp_remote_tracker::check_timeout, this));
        }

        void stop()
        {
            this->stopped = true;
            this->state = tracker_state::stopped;
        }

    private:
        virtual std::shared_ptr<std::vector<char>> get_handshake_msg() const override { return {}; }
        virtual std::size_t get_handshake_msg_len() const override { return 0; }

        void start_write_handshake()
        {
            if (this->stopped)
                return;

            auto handshake_msg = get_handshake_msg();

            this->sock.async_send(boost::asio::buffer(*handshake_msg),
                                  boost::bind(&udp_remote_tracker::handle_write_handshake,
                                              this,
                                              boost::asio::placeholders::error,
                                              boost::asio::placeholders::bytes_transferred));
        }

        void handle_write_handshake(const boost::system::error_code& err, std::size_t bytes_transferred) {}

        void start_connect(boost::asio::ip::udp::resolver::results_type endpoints)
        {
            if (endpoints.begin() != endpoints.end())
            {
                this->state = tracker_state::resolved;
                this->deadline.expires_after(std::chrono::seconds(CONNECT_DEADLINE_SECONDS));

                boost::asio::async_connect(sock,
                                           endpoints,
                                           boost::bind(&udp_remote_tracker::handle_connect,
                                                       this,
                                                       boost::asio::placeholders::error,
                                                       boost::asio::placeholders::endpoint));
            }
            else
            {
                // there were endpoints passed to this, we shouldn't have gotten this far
                stop();
            }
        }

        void handle_retry_connect(const boost::system::error_code& err) 
        { 
            if (!err)
            {
                if (this->sock.is_open())
                {
                    start_write_handshake();
                }
                else
                {
                    this->state = tracker_state::timed_out;
                    this->stopped = true;
                }
            }
            else
            {
                // there was an error so now we need to shut this  guy down
                this->sock.close();
                stop();
            }
        }

        void handle_connect(const boost::system::error_code& err, const boost::asio::ip::udp::endpoint& endpoint)
        {
            if (this->stopped)
                return;

            if (!err) // there was no error
            {
                // async_connect should have opened the socket
                if (!this->sock.is_open())
                {
                    // the socket is not open so our connection must have timed out
                    this->state = tracker_state::timed_out;
                    // reset the deadline
                    this->deadline.expires_after(std::chrono::seconds(CONNECT_DEADLINE_SECONDS));
                    this->sock.async_connect(endpoint,
                                             boost::bind(&udp_remote_tracker::handle_retry_connect,
                                                         this,
                                                         boost::asio::placeholders::error));
                    return;
                }
                else
                {
                    this->endpoint = endpoint;
                    start_write_handshake();
                }
            }
            else if (err == boost::asio::error::not_found)
            {
                // there were no endpoints for us to connect to, our resolution failed but we somehow still got here
                stop();
                return;
            }
            else
            {
                // there was some other error
                // We need to close the socket used in the previous connection attempt
                // before starting a new one.

                this->sock.close();
            }
        }

        void check_timeout()
        {
            if (this->stopped)
                return;

            if (this->deadline.expiry() <= boost::asio::steady_timer::clock_type::now())
            {
                // the deadline passed. close the socket, this will also cancel any outstanding async operations
                this->sock.close();
                this->deadline.expires_at(boost::asio::steady_timer::time_point::max());
            }

            this->deadline.async_wait(boost::bind(&udp_remote_tracker::check_timeout, this));
        }

        tracker_state state;
        boost::asio::io_context* m_io_ctx{nullptr};
        boost::asio::ip::udp::socket sock;
        boost::asio::steady_timer deadline;
        // boost::asio::ip::udp::resolver::results_type m_endpoints;
        boost::asio::ip::udp::endpoint endpoint;
        bool stopped;
        std::function<int(const std::vector<std::string>&)> m_get_peers_callback;
    };

    constexpr auto s = sizeof(boost::asio::ip::udp::socket);
} // namespace storrent
