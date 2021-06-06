#include "storrent/remote_tracker.hpp"
#include "storrent/torrent_session.hpp"
#include "storrent/tracker_manager.hpp"
#include <random>

namespace storrent
{
    //namespace
    //{
    //    std::random_device rand_seed;
    //    std::mt19937_64 big_mersenne_engine = std::mt19937_64(rand_seed());
    //    std::mt19937 mersenne_engine = std::mt19937(rand_seed());
    //} // namespace

    //remote_tracker::remote_tracker(std::shared_ptr<tracker_manager> trackers_ptr)
    //    : trackers_ptr{trackers_ptr}, connection_id{0}
    //{
    //}

    //std::int64_t remote_tracker::make_big_tx_id() { return big_mersenne_engine(); }

    //std::int32_t remote_tracker::make_small_tx_id() { return mersenne_engine(); }

    //udp_remote_tracker::udp_remote_tracker(boost::asio::io_context& io_ctx,
    //                                       std::shared_ptr<tracker_manager> trackers_ptr)
    //    : remote_tracker(trackers_ptr), sock(io_ctx), deadline(io_ctx), stopped{false}
    //{

    //}

    //void udp_remote_tracker::start_socket(boost::asio::ip::udp::resolver::results_type endpoints)
    //{
    //    start_socket_connect(endpoints);
    //    deadline.async_wait(boost::bind(&udp_remote_tracker::check_timeout, this));
    //}

    //void udp_remote_tracker::announce(std::int32_t announce_event,
    //                                  std::function<void(const boost::system::error_code& err,
    //                                                     std::size_t bytes_transferred)> announce_response_handler)
    //{
    //    // we need to check if we are connected
    //    // we need to check if we are already in the middle of an announce stream, if we're doing anything but the 
    //    // norm we can interrupt the interval they gave us. 
    //    announce_event = announce_event;
    //    announce_handler = announce_response_handler;
    //    if (!tracker_connected())
    //    {
    //        start_write_connect();
    //    }
    //    else
    //    {
    //        do_announce();
    //    }
    //}

    //void udp_remote_tracker::stop_socket()
    //{
    //    this->sock_state = tracker_sock_state::stopped;
    //    this->sock.close();
    //}

    //void udp_remote_tracker::do_connect_then_announce() 
    //{

    //}

    //void udp_remote_tracker::do_announce() {}


    //void udp_remote_tracker::start_write_connect()
    //{
    //    if (this->stopped)
    //        return;

    //    reset_transaction();
    //    reset_connection();
    //    setup_connect_packet();

    //    this->sock.async_send(boost::asio::buffer(out_data.get(), 16),
    //                          boost::bind(&udp_remote_tracker::handle_send_connect_req,
    //                                      this,
    //                                      boost::asio::placeholders::error,
    //                                      boost::asio::placeholders::bytes_transferred));
    //}

    //void udp_remote_tracker::handle_send_connect_req(const boost::system::error_code& err,
    //                                                 std::size_t bytes_transferred)
    //{
    //    if (!err)
    //    {
    //        // makes sure that all 16 bytes were sent
    //        if (bytes_transferred > 0)
    //        {
    //            // we sent the full message, now we can worry about receiving
    //            this->sock.async_receive(boost::asio::buffer(in_data),
    //                                     boost::bind(&udp_remote_tracker::handle_recv_connect_response,
    //                                                 this,
    //                                                 boost::asio::placeholders::error,
    //                                                 boost::asio::placeholders::bytes_transferred));
    //        }
    //    }
    //    else
    //    {
    //        // there was an error
    //        std::cerr << utils::make_str("error: ", err.message()) << std::endl;
    //    }
    //}

    //void udp_remote_tracker::handle_recv_connect_response(const boost::system::error_code& err,
    //                                                      std::size_t bytes_transferred)
    //{
    //    bool tracker_err = false;
    //    if (!err)
    //    {
    //        std::int32_t action;
    //        read_reverse_from_buffer(action, 4, in_data.data(), 0);
    //        std::int32_t tx;
    //        read_reverse_from_buffer(tx, 4, in_data.data(), 4);

    //        if (action == ACTION_CONNECT && bytes_transferred == 16)
    //        {
    //            // all is well
    //            std::int64_t connect_id;
    //            read_reverse_from_buffer(connect_id, 8, in_data.data(), 8);
    //            // if our tansaction ids match then we're golden, lets do the announce and get some peers up in this
    //            // bitch
    //            if (tx == this->tx_id)
    //            {
    //                // our transactions match so lets assign the connection id
    //                this->connection_id = connect_id;
    //                // send announce none because we haven't actually started downloading yet
    //                do_announce();
    //            }
    //            else
    //            {
    //                // if our transaction id's don't match then there is an issue that needs to be fixed on the
    //                // engineering level it is maybe possible that we have another torrent session waiting for a
    //                // connection response from the same server
    //                std::cerr << "WE HAVE AMAJOR PRROBLEM!!, THERE ARE TWO TORRENT SESSIONS WAITING FOR A CONNECT "
    //                             "RESPONSE FROM THE SAME SERVER AT THE SAME TIME"
    //                          << std::endl;
    //            }
    //        }
    //        else
    //        {
    //            auto diff = bytes_transferred - 8;
    //            std::string err(diff, '\0');
    //            read_reverse_from_buffer(err, diff, in_data.data(), 8);
    //            std::cerr << "tracker error: " << err << std::endl;
    //        }
    //    }
    //    else
    //    {
    //        std::cerr << utils::make_str("error: ", err.message()) << std::endl;
    //    }
    //}

    //void udp_remote_tracker::setup_connect_packet()
    //{
    //    out_data.reset();
    //    out_data = std::make_unique<char[]>(16);
    //    write_reverse_to_buffer(PROTOCOL_ID, 8, this->out_data.get(), 0);
    //    write_reverse_to_buffer(ACTION_CONNECT, 4, this->out_data.get(), 8);
    //    write_reverse_to_buffer(this->tx_id, 4, this->out_data.get(), 12);
    //}

    //void udp_remote_tracker::setup_announce_packet(std::int32_t announce_event)
    //{
    //    out_data.reset();
    //    out_data = std::make_unique<char[]>(96);
    //    auto buf = out_data.get();
    //    const torrent_session& sesh = this->trackers_ptr->sesh();

    //    // connection_id 
    //    write_reverse_to_buffer(this->connection_id, 8, buf, 0);
    //    // action
    //    write_reverse_to_buffer(ACTION_ANNOUNCE, 4, buf, 8);
    //    // transaction id
    //    write_reverse_to_buffer(this->tx_id, 4, buf, 12);
    //    // info hash
    //    write_reverse_to_buffer(sesh.tor().info_hash_val().bytes(), 20, buf, 16);
    //    // peer id
    //    write_reverse_to_buffer(get_peer_id(), 20, buf, 36);
    //    // bytes downloaded this session
    //    write_reverse_to_buffer(sesh.get_downloaded_bytes_count(), 8, buf, 56);
    //    // bytes remaining to download
    //    write_reverse_to_buffer(sesh.get_bytes_left_count(), 8, buf, 64);
    //    // bytes uploaded this session
    //    write_reverse_to_buffer(sesh.get_uploaded_bytes_count(), 8, buf, 72);
    //    // the announce event
    //    write_reverse_to_buffer(announce_event, 4, buf, 80);
    //    // client ip
    //    write_reverse_to_buffer(sesh.client_ip_addr(), 4, buf, 84);
    //    // random key- if we announce to both v4 and v6 addressses then we should use the same key for both protocols
    //    std::string random_key(4, '\0');
    //    utils::generate_random_byte_str(random_key.data(), 0, 4);
    //    write_reverse_to_buffer(random_key.c_str(), 4, buf, 88);
    //    // number of peers wanted
    //    write_reverse_to_buffer(sesh.peers_wanted(), 4, buf, 92);
    //    write_reverse_to_buffer(sesh.client_port(), 2, buf, 96);
    //}

    //void udp_remote_tracker::start_socket_connect(boost::asio::ip::udp::resolver::results_type endpoints)
    //{
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
    //}

    //void udp_remote_tracker::handle_retry_socket_connect(const boost::system::error_code& err)
    //{
    //    if (!err)
    //    {
    //        if (this->sock.is_open())
    //        {
    //            this->sock_state = tracker_sock_state::connected;
    //            // start_write_connect();
    //        }
    //        else
    //        {
    //            this->sock_state = tracker_sock_state::timed_out;
    //        }
    //    }
    //    else
    //    {
    //        // there was an error so now we need to shut this  guy down
    //        // there was an error
    //        std::cerr << utils::make_str("error: ", err.message()) << std::endl;
    //        this->sock.close();
    //        stop_socket();
    //    }
    //}

    //void udp_remote_tracker::handle_socket_connect(const boost::system::error_code& err,
    //                                        const boost::asio::ip::udp::endpoint& endpoint)
    //{
    //    if (this->stopped)
    //        return;

    //    if (!err) // there was no error
    //    {
    //        // async_connect should have opened the socket
    //        if (!this->sock.is_open())
    //        {
    //            // the socket is not open so our connection must have timed out
    //            this->sock_state = tracker_sock_state::timed_out;
    //            // reset the deadline
    //            this->deadline.expires_after(std::chrono::seconds(CONNECT_DEADLINE_SECONDS));
    //            this->sock.async_connect(endpoint,
    //                                     boost::bind(&udp_remote_tracker::handle_retry_socket_connect,
    //                                                 this,
    //                                                 boost::asio::placeholders::error));
    //            return;
    //        }
    //        else
    //        {
    //            this->endpoint = endpoint;
    //            this->sock_state = tracker_sock_state::connected;
    //            // start_write_connect();*/
    //        }
    //    }
    //    else if (err == boost::asio::error::not_found)
    //    {
    //        // there were no endpoints for us to connect to, our resolution failed but we somehow still got here
    //        // there was an error
    //        std::cerr << utils::make_str("error: ", err.message()) << std::endl;
    //        stop_socket();
    //        return;
    //    }
    //    else
    //    {
    //        // there was some other error
    //        // We need to close the socket used in the previous connection attempt
    //        // before starting a new one.
    //        // there was an error
    //        std::cerr << utils::make_str("error: ", err.message()) << std::endl;
    //        this->sock.close();
    //    }
    //}

    //void udp_remote_tracker::check_timeout()
    //{
    //    if (this->stopped)
    //        return;

    //    if (this->deadline.expiry() <= boost::asio::steady_timer::clock_type::now())
    //    {
    //        // the deadline passed. close the socket, this will also cancel any outstanding async operations
    //        this->sock.close();
    //        this->deadline.expires_at(boost::asio::steady_timer::time_point::max());
    //    }

    //    this->deadline.async_wait(boost::bind(&udp_remote_tracker::check_timeout, this));
    //}

} // namespace storrent

/*{


std::uniform_int_distribution<std::uint64_t> dist(std::llround(std::pow(2, 61)),
                                              std::llround(std::pow(2, 62)));
return dist(mersenne_engine);
}*/