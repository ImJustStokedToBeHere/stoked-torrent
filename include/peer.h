#pragma once
#include <set>
#include <cstdint>

namespace stoked {
    struct peer_endpoint {
        uint32_t addr_bytes;// [4] {0,0,0,0};
        uint16_t port;
    };

    inline constexpr bool operator<(const peer_endpoint& l, const peer_endpoint& r) {
        return l.addr_bytes < r.addr_bytes ? true : l.addr_bytes == r.addr_bytes ? l.port < r.port : false;
    }

    inline constexpr bool operator>(const peer_endpoint& l, const peer_endpoint& r) {
        return l.addr_bytes > r.addr_bytes ? true : l.addr_bytes == r.addr_bytes ? l.port > r.port : false;
    }

    inline constexpr bool operator==(const peer_endpoint& l, const peer_endpoint& r) {
        return !(l < r) && !(l > r);
    }

    inline constexpr bool operator!=(const peer_endpoint& l, const peer_endpoint& r) {
        return !(l == r);
    }

    class peer_manager {
    public:
        using iterator = std::set<peer_endpoint>::iterator;
        using const_iterator = std::set<peer_endpoint>::const_iterator;

        peer_manager() = default;
        peer_manager(const peer_manager&) = delete;
        peer_manager(peer_manager&&) = default;
        peer_manager& operator=(const peer_manager&) = delete;
        peer_manager& operator=(peer_manager&&) = default;

        bool insert(peer_endpoint ep) {
            auto ret_val = _endpoints.emplace(std::move(ep));
            return ret_val.second;
        }
        //
        size_t insert(iterator first, iterator last) {
            size_t result{0};
            for (auto itr : std::set<peer_endpoint>{first, last}) {
                if (insert(itr))
                    result++;
            }

            return result;
        }

        iterator begin() {
            return _endpoints.begin();
        }

        iterator end() {
            return _endpoints.end();
        }

        const_iterator cbegin() const {
            return _endpoints.cbegin();
        }

        const_iterator cend() const {
            return _endpoints.cend();
        }

    private:
        std::set<peer_endpoint> _endpoints;
    };

}