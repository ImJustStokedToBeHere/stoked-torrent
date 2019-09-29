#pragma once
#include <tracker.h>
#include <string>
#include <array>
#include <map>

namespace stoked {

    struct udp_tracker : public tracker {
    public:
        static constexpr int64_t INITIAL_CONNECTION_ID = 0x41727101980;
        static constexpr int32_t CONNECT_ACTION = 0x00;
        static constexpr int32_t ANNOUNCE_ACTION = 0x01;
        static constexpr int32_t SCRAPE_ACTION = 0x02;
        static constexpr int32_t ERROR_ACTION = 0x03;
        static constexpr int32_t ERROR_ACTION_TIMEOUT = ERROR_ACTION | 0x08;
        static constexpr int32_t ERROR_ACTION_MISMATCHED_IDS = ERROR_ACTION | 0x10;
        static constexpr int32_t ERROR_ACTION_PARTIAL_DGRAM_SEND = ERROR_ACTION | 0x20;
        static constexpr int32_t ERROR_ACTION_PARTIAL_DGRAM_RECV = ERROR_ACTION | 0x40;
        static constexpr int32_t ERROR_ACTION_EXCEPT = ERROR_ACTION | 0x80;
        static constexpr int32_t ERROR_ACTION_SOCKET_ERR = ERROR_ACTION | 0x100;
        static constexpr int32_t ADDR_LEN = sizeof(SOCKADDR);
        static const std::map<const int32_t, const char*> action_strings;

        udp_tracker(tracker_type type, std::string url_str, const uri& url);
        udp_tracker(udp_tracker&&) = default;
        udp_tracker(const udp_tracker&) = delete;

        virtual udp_tracker& operator=(udp_tracker&&) = default;
        udp_tracker& operator=(const udp_tracker&) = delete;

        // Inherited via tracker
        virtual scrape_result scrape(session* sess) override;
        virtual announce_result announce(session* sess, announce_event action) override;

        void shutdown(tracker_status new_status = tracker_status::down) {
            _status = new_status;
        }

        SOCKET client_socket() {
            return _client_socket;
        }

        const SOCKADDR_IN* server_addr() {
            return &_server_addr;
        }

        std::time_t connection_id_expiry() {
            return _connection_id_expiry;
        }

        int64_t get_connection_id();

        int64_t connection_id() {
            if (_status == tracker_status::down || _retry_count == 4) {
                return 0;
            } else if (utils::timestamp() > _connection_id_expiry) {
                return _connection_id;
            } else {
                return INITIAL_CONNECTION_ID;
            }
        }
    private:
        int64_t _connection_id{INITIAL_CONNECTION_ID};
        std::time_t _connection_id_expiry{0};
        SOCKET _client_socket{};
        SOCKADDR_IN _server_addr{};

        int32_t random_transaction_id() {
            return utils::mersenne_twister_random(0, INT32_MAX);
        }
    };
}

