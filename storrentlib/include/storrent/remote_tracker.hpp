#pragma once
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
        dead,
        ok
    };

    // typedef int (*handle_peers_func)(const std::vector<std::string>& peers);

    typedef std::function<int(const std::vector<std::string>&)> handle_peers_func;

    struct remote_tracker
    {
        static constexpr auto CONNECT_DEADLINE_SECONDS = 35;
        remote_tracker(torrent_handle htor) : m_htor{htor} {}
        virtual ~remote_tracker() {}

    protected:
        virtual std::vector<char> get_handshake_msg() const = 0;
        virtual size_t get_handshake_msg_len() const = 0;
        torrent_handle m_htor{nullptr};
    };

    struct udp_remote_tracker : public remote_tracker
    {
        udp_remote_tracker(boost::asio::io_context* io_ctx, torrent_handle htor)
            : remote_tracker(htor),
              m_sock(*io_ctx),
              m_connect_deadline(*io_ctx),
              m_stopped{false},
              m_state{tracker_state::none}
        {
        }

        void start(boost::asio::ip::udp::resolver::results_type endpoints, handle_peers_func get_peers_callback)
        {
            m_get_peers_callback = get_peers_callback;
            m_endpoints = endpoints;
            start_connect(endpoints.begin());
            m_connect_deadline.async_wait(boost::bind(&udp_remote_tracker::check_timeout, this));
        }

        void stop() {}

    protected:
        virtual std::vector<char> get_handshake_msg() const override { return {}; }
        virtual size_t get_handshake_msg_len() const override { return 0; }

    private:
        void start_connect(boost::asio::ip::udp::resolver::results_type::iterator endpoint_itr)
        {
            if (endpoint_itr != m_endpoints.end())
            {
                m_connect_deadline.expires_after(std::chrono::seconds(CONNECT_DEADLINE_SECONDS));
                m_sock.async_connect(endpoint_itr->endpoint(),
                                     boost::bind(&udp_remote_tracker::handle_connect,
                                                 this,
                                                 boost::asio::placeholders::error,
                                                 endpoint_itr));
            }
            else
            {
            }
        }

        void handle_connect(const boost::system::error_code& err,
                            boost::asio::ip::udp::resolver::results_type::iterator endpoint_itr)
        {
            if (m_stopped || m_state == tracker_state::dead)
                return;

            // if the socket is closed when we get here then we must have timed out first so try the next endpoint
            if (!m_sock.is_open())
            {
                start_connect(++endpoint_itr);
            }
            else if (err)
            {
                // there was a connection error
            }
            else
            {
                // we connected sucessfully, YAY!!!
                m_endpoint = endpoint_itr->endpoint();
                m_sock.async_send(boost::asio::buffer(get_handshake_msg()),
                                  boost::bind(&udp_remote_tracker::handle_write,
                                              this,
                                              boost::asio::placeholders::error,
                                              boost::asio::placeholders::bytes_transferred));
            }
        }

        void handle_write(const boost::system::error_code& err, size_t bytes_sent) 
        {
            // we've written successfully
        }

        void check_timeout() {}

        tracker_state m_state;
        boost::asio::io_context* m_io_ctx{nullptr};
        boost::asio::ip::udp::socket m_sock;
        boost::asio::steady_timer m_connect_deadline;
        boost::asio::ip::udp::resolver::results_type m_endpoints;
        boost::asio::ip::udp::endpoint m_endpoint;
        bool m_stopped;
        handle_peers_func m_get_peers_callback;
    };
} // namespace storrent
