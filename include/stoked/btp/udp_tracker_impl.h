#pragma once
#include <stoked/btp/udp_tracker.h>

namespace stoked::btp {
    namespace {
        struct tracker_msg {
        public:
            int32_t action{};
            int32_t transact_id{};
            tracker_msg()
                : action{udp_tracker::ERROR_ACTION},
                transact_id{0}
            {}

            tracker_msg(int32_t action, int32_t transact_id)
                : action{action},
                transact_id{transact_id}
            { }

            virtual void host_to_network_bytes(char* bytes) const = 0;
            virtual void network_to_host_bytes(char* bytes, size_t len = 0) = 0;
            virtual size_t call_length() const = 0;
            virtual size_t response_length() const = 0;
        };

        struct connect_input : public tracker_msg {
        public:
            int64_t connect_id{};

            connect_input()
                : tracker_msg(udp_tracker::CONNECT_ACTION, 0) {
            }

            connect_input(int64_t connection_id, int32_t transaction_id)
                : tracker_msg(udp_tracker::CONNECT_ACTION, transaction_id),
                connect_id{connection_id}
            { }

            virtual void host_to_network_bytes(char* bytes) const override {
                auto cid = htonll(connect_id);
                auto act = htonl(action);
                auto tid = htonl(transact_id);

                memcpy(bytes, &cid, 8);
                memcpy(bytes + 8, &act, 4);
                memcpy(bytes + 12, &tid, 4);
            }

            virtual void network_to_host_bytes(char* bytes, size_t len = 0) override {
                throw std::exception("connect_input.network_order_bytes(char *bytes): is not implemented");
            }

            virtual size_t call_length() const override {
                return 16;
            }

            virtual size_t response_length() const override {
                return 16;
            }

        private:
            // mutable std::stringstream _ss {};
        };

        struct connect_output : public tracker_msg {
        public:
            int64_t connect_id{};

            connect_output()
                : tracker_msg(udp_tracker::CONNECT_ACTION, 0) {
            }

            virtual void host_to_network_bytes(char* bytes) const override {
                throw std::exception("connect_output.network_order_bytes(): is not implemented");
            }

            virtual void network_to_host_bytes(char* bytes, size_t len = 0) override {

                int32_t act{};
                memcpy(&act, bytes, 4);
                action = ntohl(act);

                int32_t tid{};
                memcpy(&tid, bytes + 4, 4);
                transact_id = ntohl(tid);

                int64_t cid{};
                memcpy(&cid, bytes + 8, 8);
                connect_id = ntohll(cid);
            }

            virtual size_t call_length() const override {
                throw std::exception("connect_output.call_length() not implemented");
            }

            virtual size_t response_length() const override {
                return 16;
            }
        };

        struct scrape_input : public tracker_msg {
        public:
            int64_t connect_id{};
            std::array<char, 20> info_hash_bytes{};

            scrape_input() :
                tracker_msg(udp_tracker::SCRAPE_ACTION, 0) {
            }

            scrape_input(int64_t connection_id, int32_t transaction_id, std::array<char, 20> info_hash_str)
                : tracker_msg(udp_tracker::SCRAPE_ACTION, transaction_id),
                connect_id{connection_id},
                info_hash_bytes{std::move(info_hash_str)}
            { }

            virtual void host_to_network_bytes(char* bytes) const override {
                auto cid = htonll(connect_id);
                auto act = htonl(action);
                auto tid = htonl(transact_id);

                std::array<char, 20> temp = info_hash_bytes;

                memcpy(bytes, &cid, 8);
                memcpy(bytes + 8, &act, 4);
                memcpy(bytes + 12, &tid, 4);
                memcpy(bytes + 16, &temp, 20);
            }

            virtual void network_to_host_bytes(char* bytes, size_t len = 0) override {
                throw std::exception("scrape_input.network_order_bytes(char *bytes): is not implemented");
            }

            virtual size_t call_length() const override {
                return 36;
            }

            virtual size_t response_length() const override {
                return 20;
            }

        };

        struct scrape_output : public tracker_msg {
        public:
            int64_t connect_id{};
            int32_t seeds{};
            int32_t downloads{};
            int32_t leeches{};

            scrape_output()
                : tracker_msg(udp_tracker::SCRAPE_ACTION, 0) {
            }

            virtual void host_to_network_bytes(char* bytes) const override {
                throw std::exception("connect_output.network_order_bytes(): is not implemented");
            }

            virtual void network_to_host_bytes(char* bytes, size_t len = 0) override {
                int32_t act{};
                memcpy(&act, bytes, 4);
                action = ntohl(act);

                int32_t tid{};
                memcpy(&tid, bytes + 4, 4);
                transact_id = ntohl(tid);

                int32_t complete{};
                memcpy(&complete, bytes + 8, 4);
                seeds = ntohl(complete);

                int32_t downloaded{};
                memcpy(&downloaded, bytes + 12, 4);
                downloads = ntohl(downloaded);

                int32_t incomplete{};
                memcpy(&incomplete, bytes + 16, 4);
                leeches = ntohl(incomplete);
            }

            virtual size_t call_length() const override {
                throw std::exception("connect_output.call_length() not implemented");
            }

            virtual size_t response_length() const override {
                return 20;
            }
        };

        struct announce_input : public tracker_msg {
        public:
            int64_t connect_id{};
            std::array<char, 20> info_hash_bytes{};
            std::array<char, 20> peer_id{};
            int64_t downloaded{0};
            int64_t left{0};
            int64_t uploaded{0};
            int32_t event_type{announce_event::announce_none};
            uint32_t ip{0};
            // this key should be unique per torrent file incase 
            // the same tracker is used for multiple torrent downloads
            uint32_t client_key{0};
            int32_t num_want{0};
            int16_t port{0};
            int16_t ext{0};

            announce_input() :
                tracker_msg(udp_tracker::ANNOUNCE_ACTION, 0) {
            }

            announce_input(int64_t connection_id,
                           int32_t transaction_id,
                           std::array<char, 20> info_hash_inp,
                           std::array<char, 20> peer_id_inp,
                           int64_t bytes_downloaded,
                           int64_t bytes_left,
                           int64_t bytes_uploaded,
                           int32_t event_type,
                           uint32_t ip_addr,
                           uint32_t unique_client_key,
                           int32_t num_peers_want,
                           int16_t port_num,
                           int16_t extensions)
                : tracker_msg(udp_tracker::ANNOUNCE_ACTION, transaction_id),
                connect_id{connection_id},
                downloaded{bytes_downloaded},
                left{bytes_left},
                uploaded{bytes_uploaded},
                event_type{event_type},
                ip{ip_addr},
                client_key{unique_client_key},
                num_want{num_peers_want},
                port{port_num},
                ext{extensions},
                info_hash_bytes{std::move(info_hash_inp)},
                peer_id{std::move(peer_id_inp)}
            { }

            virtual void host_to_network_bytes(char* bytes) const override {
                auto cid = htonll(connect_id);
                auto act = htonl(action);
                auto tid = htonl(transact_id);

                std::array<char, 20> temp = info_hash_bytes;
                /*std::copy(info_hash_bytes.begin(), info_hash_bytes.end(), &temp); */
                // std::reverse(temp.begin(), temp.end());

                std::array<char, 20> temp_peer_id = peer_id;
                // std::copy(peer_id.begin(), peer_id.end(), &temp_peer_id);
                // std::reverse(temp_peer_id.begin(), temp_peer_id.end());

                auto dl = htonll(downloaded);
                auto l = htonll(left);
                auto ul = htonll(uploaded);
                auto eve = htonl(event_type);
                auto my_ip = htonl(ip);
                auto key = htonl(client_key);
                auto want = htonl(num_want);
                auto p = htons(port);
                auto e = htons(ext);

                memcpy(bytes, &cid, 8);
                memcpy(bytes + 8, &act, 4);
                memcpy(bytes + 12, &tid, 4);
                memcpy(bytes + 16, &temp, 20);
                memcpy(bytes + 36, &temp_peer_id, 20);
                memcpy(bytes + 56, &dl, 8);
                memcpy(bytes + 64, &l, 8);
                memcpy(bytes + 72, &ul, 8);
                memcpy(bytes + 80, &eve, 4);
                memcpy(bytes + 84, &my_ip, 4);
                memcpy(bytes + 88, &key, 4);
                memcpy(bytes + 92, &want, 4);
                memcpy(bytes + 96, &p, 2);
                memcpy(bytes + 98, &e, 2);
            }

            virtual void network_to_host_bytes(char* bytes, size_t len = 0) override {
                throw std::exception("scrape_input.network_order_bytes(char *bytes): is not implemented");
            }

            virtual size_t call_length() const override {
                return 100;
            }

            virtual size_t response_length() const override {
                return 20;
            }

        };

        struct announce_output : public tracker_msg {        
            int32_t interval{};
            int32_t leechers{};
            int32_t seeders{};
            std::shared_ptr<peer_endpoint[]> peers{nullptr};

            announce_output()
                : tracker_msg(udp_tracker::ANNOUNCE_ACTION, 0) {
            }

            virtual void host_to_network_bytes(char* bytes) const override {
                throw std::exception("connect_output.network_order_bytes(): is not implemented");
            }

            virtual void network_to_host_bytes(char* bytes, size_t len = 0) override {
                std::string debug{bytes, bytes + len};
                int32_t act{};
                memcpy(&act, bytes, 4);
                action = ntohl(act);

                int32_t tid{};
                memcpy(&tid, bytes + 4, 4);
                transact_id = ntohl(tid);

                int32_t intv{};
                memcpy(&intv, bytes + 8, 4);
                interval = ntohl(intv);

                int32_t l{};
                memcpy(&l, bytes + 12, 4);
                leechers = ntohl(l);

                int32_t s{};
                memcpy(&s, bytes + 16, 4);
                seeders = ntohl(s);

                size_t count = len - 20;
                size_t n = count / 6;

                peers = std::unique_ptr<peer_endpoint[]>(new peer_endpoint[n]);

                for (size_t idx = 20, i = 0; i < n; i++, idx += 6) {
                    uint32_t addr{0};

                    memcpy(&addr, bytes + idx, 4);

                    struct in_addr ip_addr;
                    ip_addr.s_addr = addr;



                    peer_endpoint& p = peers.get()[i];

                    p.addr_bytes = ntohl(addr);

                    uint16_t port{0};
                    memcpy(&(port), bytes + idx + 4, 2);
                    p.port = ntohs(port);

                    OutputDebugString(utils::make_str("peer ip: ", utils::ip_str(ip_addr), ":", p.port, "\n").c_str());
                }
            }

            virtual size_t call_length() const override {
                throw std::exception("connect_output.call_length() not implemented");
            }

            virtual size_t response_length() const override {
                return 20;
            }
        };

        struct err_output : public tracker_msg {        
            std::string msg{};
            std::string server_url{};

            err_output() :
                tracker_msg(udp_tracker::ERROR_ACTION, 0) {
            }

            err_output(int32_t error, int32_t transaction_id, std::string msg, std::string server_url)
                : tracker_msg(error, transaction_id),
                msg{std::move(msg)},
                server_url{std::move(server_url)}
            { }

            virtual void host_to_network_bytes(char* bytes) const override {
                throw std::exception("connect_input.network_order_bytes(char *bytes): is not implemented");
            }

            virtual void network_to_host_bytes(char* bytes, size_t len = 0) override {
                throw std::exception("connect_input.network_order_bytes(char *bytes): is not implemented");
            }

            virtual size_t call_length() const override {
                throw std::exception("connect_output.call_length() not implemented");
            }

            virtual size_t response_length() const override {
                return 0;
            }
        };

        std::shared_ptr<tracker_msg> send_request(const tracker_msg& msg, SOCKET client_socket, const SOCKADDR_IN* server_addr) {
            static const size_t RECV_BUFFER_SIZE = 65535 / 2 / 2;
            try {
                std::unique_ptr<char[]> data = std::unique_ptr<char[]>(new char[msg.call_length()]);
                msg.host_to_network_bytes(data.get());
                if (auto sent_bytes = sendto(client_socket, data.get(), msg.call_length(), 0, (const SOCKADDR*)server_addr, udp_tracker::ADDR_LEN); sent_bytes == msg.call_length()) { // do some socket send here with a timeout					
                    char result[RECV_BUFFER_SIZE];
                    ZeroMemory(result, RECV_BUFFER_SIZE);
                    SOCKADDR addr{*((const SOCKADDR*)server_addr)};
                    int len{sizeof(SOCKADDR_IN)};

                    FD_SET read_set;
                    FD_ZERO(&read_set);
                    FD_SET(client_socket, &read_set);

                    FD_SET write_set;
                    FD_ZERO(&write_set);

                    timeval timeout{15, 0};

                    if (auto select_val = select(-1, &read_set, &write_set, NULL, &timeout); select_val != SOCKET_ERROR) {
                        if (select_val == 1 && FD_ISSET(client_socket, &read_set)) {
                            if (auto recv_bytes = recvfrom(client_socket, result, RECV_BUFFER_SIZE, 0, &addr, &len); recv_bytes >= static_cast<int>(msg.response_length())) { // do some socket recv with a timeout here

                                std::shared_ptr<tracker_msg> output{nullptr};

                                switch (msg.action) {
                                    case udp_tracker::CONNECT_ACTION:
                                    {
                                        output = std::make_shared<connect_output>();
                                        break;
                                    }
                                    case udp_tracker::SCRAPE_ACTION:
                                    {
                                        output = std::make_shared<scrape_output>();
                                        break;
                                    }
                                    case udp_tracker::ANNOUNCE_ACTION:
                                    {
                                        output = std::make_shared<announce_output>();
                                        break;
                                    }
                                    default:
                                        throw std::exception(utils::make_str("udp_tracker.send_request: invalid tracker action ", msg.action).c_str());
                                }

                                // std::string s {result};
                                // std::reverse(s.begin(), s.end());
                                if (recv_bytes != RECV_BUFFER_SIZE) {
                                    // only if we haven't maxed out our buffer then 
                                    // we need to write a null to terminate our data
                                    // becuase we use it like a string
                                    result[recv_bytes] = '\0';
                                }
                                
                                output->network_to_host_bytes(result, recv_bytes);

                                if (output->transact_id == msg.transact_id) {
                                    return output;
                                } else {
                                    return std::make_shared<err_output>(udp_tracker::ERROR_ACTION_MISMATCHED_IDS,
                                                                        msg.transact_id,
                                                                        utils::make_str(udp_tracker::action_strings.at(udp_tracker::ERROR_ACTION_MISMATCHED_IDS),
                                                                                        ", send: ",
                                                                                        msg.transact_id,
                                                                                        "recv: ",
                                                                                        output->transact_id),
                                                                        utils::ip_str(server_addr->sin_addr));
                                }
                            } else if (recv_bytes == SOCKET_ERROR) {
                                return std::make_shared<err_output>(udp_tracker::ERROR_ACTION_SOCKET_ERR,
                                                                    msg.transact_id,
                                                                    utils::make_str(udp_tracker::action_strings.at(udp_tracker::ERROR_ACTION_SOCKET_ERR),
                                                                                    ", error code: ", utils::last_socket_err(),
                                                                                    " msg: ", utils::last_socket_err_str()),
                                                                    utils::ip_str(server_addr->sin_addr));

                            } else if (recv_bytes < static_cast<int>(msg.response_length())) {
                                return std::make_shared<err_output>(udp_tracker::ERROR_ACTION_PARTIAL_DGRAM_RECV,
                                                                    msg.transact_id,
                                                                    utils::make_str(udp_tracker::action_strings.at(udp_tracker::ERROR_ACTION_PARTIAL_DGRAM_RECV),
                                                                                    ", min response bytes: ", msg.response_length(),
                                                                                    "recv bytes: ", recv_bytes),
                                                                    utils::ip_str(server_addr->sin_addr));
                            }

                        } else {
                            OutputDebugString(utils::make_str(utils::ip_str(server_addr->sin_addr), " timed out (1)", "\n").c_str());
                            return std::make_shared<err_output>(udp_tracker::ERROR_ACTION_TIMEOUT,
                                                                msg.transact_id,
                                                                udp_tracker::action_strings.at(udp_tracker::ERROR_ACTION_TIMEOUT),
                                                                utils::ip_str(server_addr->sin_addr));
                        }
                    } else {
                        OutputDebugString(utils::make_str(utils::ip_str(server_addr->sin_addr), " timed out (2)", "\n").c_str());
                        return std::make_shared<err_output>(udp_tracker::ERROR_ACTION_TIMEOUT,
                                                            msg.transact_id,
                                                            udp_tracker::action_strings.at(udp_tracker::ERROR_ACTION_TIMEOUT),
                                                            utils::ip_str(server_addr->sin_addr));
                    }
                } else if (sent_bytes == SOCKET_ERROR) {
                    return std::make_shared<err_output>(udp_tracker::ERROR_ACTION_SOCKET_ERR,
                                                        msg.transact_id,
                                                        utils::make_str(udp_tracker::action_strings.at(udp_tracker::ERROR_ACTION_SOCKET_ERR),
                                                                        ", error code: ", utils::last_socket_err(),
                                                                        " msg: ", utils::last_socket_err_str()),
                                                        utils::ip_str(server_addr->sin_addr));

                } else if (sent_bytes < static_cast<int>(msg.call_length())) {
                    return std::make_shared<err_output>(udp_tracker::ERROR_ACTION_PARTIAL_DGRAM_SEND,
                                                        msg.transact_id,
                                                        utils::make_str(udp_tracker::action_strings.at(udp_tracker::ERROR_ACTION_PARTIAL_DGRAM_SEND),
                                                                        ", min response bytes: ", msg.response_length(),
                                                                        "sent bytes: ", sent_bytes),
                                                        utils::ip_str(server_addr->sin_addr));
                } else {
                    return std::make_shared<err_output>(udp_tracker::ERROR_ACTION_TIMEOUT,
                                                        msg.transact_id,
                                                        udp_tracker::action_strings.at(udp_tracker::ERROR_ACTION_TIMEOUT),
                                                        utils::ip_str(server_addr->sin_addr));
                }
            } catch (const std::exception & e) {
                return std::make_shared<err_output>(udp_tracker::ERROR_ACTION_EXCEPT,
                                                    msg.transact_id,
                                                    utils::make_str(udp_tracker::action_strings.at(udp_tracker::ERROR_ACTION_EXCEPT),
                                                                    ", what: ", e.what()),
                                                    utils::ip_str(server_addr->sin_addr));
            }

            return std::shared_ptr<err_output>(nullptr);
        }
    }
}
