#include <tracker.h>
#include <udp_tracker.h>

namespace stoked {

    std::shared_ptr<tracker> tracker::create_tracker(const std::string& tracker_url) {
        tracker_type type;
        if (auto url = uri::parse(tracker_url); url.has_value()) {
            std::string protocol = url->protocol();
            if (protocol == "udp") {
                type = tracker_type::udp;
                return std::make_shared<udp_tracker>(type, tracker_url, url.value());
            } else if (protocol == "http") {
                type = tracker_type::http;
                return std::make_shared<udp_tracker>(type, tracker_url, url.value());
            }
        } else {
            return nullptr;
        }
    }

    scrape_result null_tracker::scrape(session* sess) {
        return scrape_result();
    }

    announce_result null_tracker::announce(session* sess, announce_event action) {
        return announce_result();
    }


}