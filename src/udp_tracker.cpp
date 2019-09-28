#include <udp_tracker.h>
#include <session.h>
#include <peer.h>
#include <udp_tracker_impl.h>


namespace stoked {

    const std::map<const int32_t, const char*> udp_tracker::action_strings{
        {CONNECT_ACTION, "connect"},
        {ANNOUNCE_ACTION, "announce"},
        {SCRAPE_ACTION, "scrape"},
        {ERROR_ACTION, "unk error"},
        {ERROR_ACTION_TIMEOUT, "timeout error"},
        {ERROR_ACTION_MISMATCHED_IDS, "id mismatch error"},
        {ERROR_ACTION_PARTIAL_DGRAM_SEND, "partial send error"},
        {ERROR_ACTION_PARTIAL_DGRAM_RECV, "partial recv error"},
        {ERROR_ACTION_SOCKET_ERR, "socket error"},
        {ERROR_ACTION_EXCEPT, "exception error"}
    };

    const announce_input& get_announce_input(session* sess, announce_input& inp) {
        return inp;
    }

    int64_t udp_tracker::get_connection_id() {
        if (server_is_down()) {
            return 0;
        } else if (utils::timestamp() < connection_id_expiry() && connection_id() != INITIAL_CONNECTION_ID) {
            return connection_id();
        } else {
            connect_input inp{
                INITIAL_CONNECTION_ID,
                random_transaction_id()
            };

            try {
                std::string u = this->url();
                auto result = send_request(inp, client_socket(), server_addr());

                if (result) {
                    if (result->action == CONNECT_ACTION) {
                        auto conn_res = static_cast<const connect_output&>(*result.get());
                        _connection_id = conn_res.connect_id;
                        _connection_id_expiry = utils::timestamp_add_minutes(1);
                        return _connection_id;
                    } else if (result->action & ERROR_ACTION) {

                        auto err_res = static_cast<const err_output&>(*result.get());

                        set_last_error(err_res.action, err_res.transact_id, err_res.msg, err_res.server_url);

                        switch (err_res.action) {
                            case ERROR_ACTION_TIMEOUT:
                            {
                                if (++_retry_count == 4) {
                                    this->shutdown(tracker_status::errored);
                                    return 0;
                                }
                                return get_connection_id();
                            }
                            case ERROR_ACTION_MISMATCHED_IDS:
                            {
                                return INITIAL_CONNECTION_ID;
                            }
                            default:
                            {
                                this->shutdown(tracker_status::errored);
                                return 0;
                            }
                        }
                    }
                } else {
                    set_last_error(
                        ERROR_ACTION_EXCEPT,
                        inp.transact_id,
                        utils::make_str("no response returned"),
                        utils::ip_str(server_addr()->sin_addr)
                    );
                    this->shutdown();
                    return INITIAL_CONNECTION_ID;
                }
            } catch (const std::exception & e) {
                set_last_error(
                    ERROR_ACTION_EXCEPT,
                    inp.transact_id,
                    utils::make_str("unhandled error type: ", e.what()),
                    utils::ip_str(server_addr()->sin_addr)
                );

                this->shutdown(tracker_status::errored);
                return 0;
            }
        }

        return 0;
    }

    udp_tracker::udp_tracker(tracker_type type, std::string url_str, const uri& url)
        : tracker(type, std::move(url_str), url) {

        std::string ip_addr{ };
        SOCKADDR_IN passed_addr{ };
        if (utils::get_ip_from_hostname(url.domain(), url.port(), IPPROTO_UDP, ip_addr, passed_addr)) {
            _server_addr = passed_addr;

            if ((_client_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) != SOCKET_ERROR) {
                sockaddr_in RecvAddr;
                RecvAddr.sin_family = AF_INET;
                RecvAddr.sin_port = htons(6969);
                RecvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

                auto err = WSAGetLastError();
                _status = tracker_status::started;
                return;
            }
        }

        _status = tracker_status::errored;
    }

    scrape_result udp_tracker::scrape(session* sess) {

        OutputDebugString(utils::make_str("connecting: ", url(), " ", utils::ip_str(_server_addr.sin_addr), "\n").c_str());

        auto connect_id = get_connection_id();

        if (server_is_down()) {
            OutputDebugString(utils::make_str("connection timed out: ", url(), " ", utils::ip_str(_server_addr.sin_addr), "\n").c_str());
            return scrape_result{
                    0,0,0,0,
                    set_last_error(
                        ERROR_ACTION_TIMEOUT,
                        0,
                        "timed out while trying to connect",
                        utils::make_str(utils::ip_str(server_addr()->sin_addr), "-", url())
                    )
            };
        }

        OutputDebugString(utils::make_str("connected: ", url(), " ", utils::ip_str(_server_addr.sin_addr), "\n").c_str());


        scrape_input inp{
            connect_id,
            random_transaction_id(),
            sess->info_hash_bytes()
        };

        try {
            auto output = send_request(inp, client_socket(), server_addr());
            if (output) {
                if (output->action == SCRAPE_ACTION) {
                    auto scrape_ret = static_cast<const scrape_output&>(*output.get());

                    if (scrape_ret.seeds == 0 && scrape_ret.leeches == 0) {
                        _status = tracker_status::dry;
                    }

                    return scrape_result{
                        scrape_ret.transact_id,
                        scrape_ret.seeds,
                        scrape_ret.downloads,
                        scrape_ret.leeches,
                        error_result {}
                    };
                } else if (output->action & ERROR_ACTION) {
                    _connection_id_expiry = utils::timestamp();
                    _retry_count++;

                    auto err = static_cast<const err_output&>(*output.get());
                    // auto err = dynamic_cast<std::shared_ptr<err_output>>(output);
                    return scrape_result{
                        inp.transact_id,
                        0,
                        0,
                        0,
                        set_last_error(
                            err.action,
                            err.transact_id,
                            err.msg,
                            err.server_url
                        )
                    };
                }
            } else {
                return scrape_result{
                    inp.transact_id, 0,0,0,
                    set_last_error(
                        ERROR_ACTION_EXCEPT,
                        inp.transact_id,
                        "joe, you fucked up",
                        utils::ip_str(server_addr()->sin_addr)
                    )
                };
            }
        } catch (const std::exception & e) {

            return scrape_result{
                inp.transact_id, 0, 0, 0,
                set_last_error(
                    ERROR_ACTION_EXCEPT,
                    inp.transact_id,
                    utils::make_str("unhandled error type: ", e.what()),
                    utils::ip_str(server_addr()->sin_addr)
                )
            };
        }

        return {};
    }

    announce_result udp_tracker::announce(session* sess, announce_event event_type) {
        if (_status == tracker_status::dry) {
            // there is no use announcing as we are dry so just exit and try to scrape again in an hour maybe
            return announce_result{};
        }

        int64_t connect_id = get_connection_id();

        announce_input inp{
            connect_id,
            random_transaction_id(),
            sess->info_hash_bytes(),
            sess->get_peer_id(),
            sess->bytes_downloaded(),
            sess->bytes_left(),
            sess->bytes_uploaded(),
            event_type,
            0,
            sess->client_key(),
            -1,
            sess->listening_port(),
            0U
        };

        try {
            auto output = send_request(inp, client_socket(), server_addr());

            if (output) {
                if (output->action == ANNOUNCE_ACTION) {

                    auto ret = static_cast<const announce_output&>(*output.get());
                    return announce_result{
                        ret.transact_id,
                        ret.interval,
                        ret.leechers,
                        ret.seeders,
                        ret.peers,
                        event_type,
                        {}
                    };
                } else if (output->action & ERROR_ACTION) {
                    _connection_id_expiry = utils::timestamp();
                    _retry_count++;

                    auto err = static_cast<const err_output&>(*output.get());
                    // auto err = dynamic_cast<std::shared_ptr<err_output>>(output);
                    return announce_result{
                        inp.transact_id,
                        0,
                        0,
                        0,
                        nullptr,
                        event_type,
                        set_last_error(
                            err.action,
                            err.transact_id,
                            err.msg,
                            err.server_url
                        )
                    };
                }
            } else {
                return announce_result{
                    inp.transact_id,
                    0,
                    0,
                    0,
                    nullptr,
                    event_type,
                    set_last_error(
                        ERROR_ACTION_EXCEPT,
                        inp.transact_id,
                        "joe, you fucked up",
                        utils::ip_str(server_addr()->sin_addr)
                    )
                };
            }
        } catch (const std::exception & e) {
            return announce_result{
                inp.transact_id,
                0,
                0,
0,
                nullptr,
                event_type,
                set_last_error(
                    ERROR_ACTION_EXCEPT,
                    inp.transact_id,
                    utils::make_str("unhandled error type: ", e.what()),
                    utils::ip_str(server_addr()->sin_addr)
                )
            };
        }

        return {};
    }

}

