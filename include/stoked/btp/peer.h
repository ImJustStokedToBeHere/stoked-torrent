#pragma once
#include <stoked/btp/utils.h>
#include <set>
#include <cstdint>

namespace stoked::btp {
    struct peer_endpoint {
        uint32_t addr_bytes;// [4] {0,0,0,0};
        uint16_t port;

        const char* str() const {
            return utils::make_str((addr_bytes & 0xFF000000) >> 24, (addr_bytes & 0x00FF0000) >> 16, (addr_bytes & 0x0000FF00) >> 8, (addr_bytes & 0x000000FF)).c_str();
        }
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
}