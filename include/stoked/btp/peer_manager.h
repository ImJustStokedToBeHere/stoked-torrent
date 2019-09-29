#pragma once
#include <stoked/btp/peer.h>
#include <gsl/span>

namespace stoked::btp {
    struct peer_manager {    
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
        size_t insert(const_iterator first, const_iterator last) {
            size_t result{0};
            for (auto itr : {first, last}) {
                if (insert(*itr))
                    result++;
            }

            return result;
        }

        size_t insert(gsl::span<peer_endpoint> endpoints) {

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