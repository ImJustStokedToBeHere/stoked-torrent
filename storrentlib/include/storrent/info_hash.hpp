#pragma once
#include "storrent/sha1.hpp"
#include "storrent/utils.hpp"
#include <array>
#include <string>
#include <utility>
#include <vector>

namespace storrent
{
    struct info_hash
    {
        info_hash() {}
        info_hash(const info_hash&) = default;
        info_hash(info_hash&&) = default;

        explicit info_hash(const ::std::string& info_str);

        explicit info_hash(sha1 checksum);

        info_hash& operator=(const info_hash&) = default;
        info_hash& operator=(info_hash&&) = default;

        
        const std::string& hex() const { return _hex_str; }

        std::vector<char> bytes_vec() const { return {_bytes.begin(), _bytes.end()}; }

        const std::array<char, 20>& bytes() const { return _bytes; }

    private:
        std::array<char, 20> _bytes{};
        std::string _hex_str{};
        // sha1 _sha1;
    };

    bool operator==(const info_hash& lhs, const info_hash& rhs);
    bool operator!=(const info_hash& lhs, const info_hash& rhs);
} // namespace storrent

namespace std
{
    template <>
    struct hash<storrent::info_hash>
    {
        std::size_t operator()(const storrent::info_hash& s) const noexcept
        {
            size_t seed{0};
            storrent::utils::hash_combine(seed, s.bytes());
            return seed;
        }
    };
} // namespace std
