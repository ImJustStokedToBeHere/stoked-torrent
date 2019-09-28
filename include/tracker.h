#pragma once
#include <utils.h>
#include <peer.h>
#include <uri.h>
#include <memory>
#include <stdint.h>
#include <limits>

namespace stoked {
    struct session;

    enum announce_event : int32_t {
        announce_none = 0x00,
        announce_completed = 0x01,
        announce_started = 0x02,
        announce_stopped = 0x03
    };

    enum class tracker_state : char {
        unknown,
        connecting,
        announcing,
        scraping,
        erroring
    };

    enum class tracker_status : char {
        started = 0x01,
        dry = 0x02 | started,
        stopped = 0x04,
        errored = 0x08,
        down = 0x10
    };

    enum class tracker_type : char {
        unknown,
        udp,
        http
    };

    struct error_result {
    public:
        int32_t action{0};
        int32_t transaction_id{0};
        std::string msg;
        std::string server_url;
    };

    struct scrape_result {
    public:
        int32_t transaction_id{0};
        int32_t connected_seeds{0};
        int32_t completed_downloads{0};
        int32_t connected_leeches{0};
        error_result error;
    };

    struct announce_result {
    public:
        int32_t transaction_id{0};
        int32_t interval{0};
        int32_t leechers{0};
        int32_t seeders{0};
        std::shared_ptr<peer_endpoint[]> peers{nullptr};
        announce_event event_type{announce_none};
        error_result error;
    };

    class tracker {
    public:
        static std::shared_ptr<tracker> create_tracker(const std::string& tracker_url);

        tracker(tracker_type type, std::string url_str, const uri& url)
            : _type{type},
            _url{std::move(url_str)} { }

        tracker(tracker&&) = default;
        tracker(const tracker&) = delete;
        virtual ~tracker() = default;

        tracker& operator=(tracker&&) = default;
        tracker& operator=(const tracker&) = delete;

        virtual scrape_result scrape(session* sess) = 0;
        virtual announce_result announce(session* sess, announce_event action) = 0;
        void set_announce_event_handler() {

        }

        virtual std::string url() {
            return _url;
        }

        /*virtual const err_output *last_error() {
            return _last_err.get();
        }*/

        virtual tracker_state state() {
            return _state;
        }

        virtual tracker_status status() {
            return _status;
        }

        virtual tracker_type type() {
            return _type;
        }

        virtual bool server_is_down() {
            return static_cast<char>(status())& (static_cast<char>(tracker_status::down) | static_cast<char>(tracker_status::errored));
        }

        virtual bool server_is_up() {
            return !server_is_down();
        }

        virtual int32_t announce_interval() const {
            return _announce_interval;
        }

        virtual time_t next_announce() const {
            return _next_announce;
        }

        virtual bool announcing() const {
            return next_announce() > std::numeric_limits<time_t>::min() && announce_interval() >= 0;
        }

        virtual error_result last_error() {
            return _last_error;
        }

    protected:

        virtual error_result set_last_error(int32_t action, int32_t transaction_id, std::string msg, std::string server_url) {
            _last_error = error_result{
                action,
                transaction_id,
                std::move(msg),
                std::move(server_url)
            };

            return _last_error;
        }


        std::string _url;
        tracker_type _type{tracker_type::unknown};
        tracker_state _state{tracker_state::unknown};
        tracker_status _status{tracker_status::stopped};
        int32_t _announce_interval{-1};
        time_t _next_announce{std::numeric_limits<time_t>::min()};
        char _retry_count{0};
        error_result _last_error;
    };

    class null_tracker : public tracker {
    public:
        virtual scrape_result scrape(session* sess) override;
        virtual announce_result announce(session* sess, announce_event action) override;
    };

}