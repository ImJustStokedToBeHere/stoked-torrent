#pragma once
#include "storrent/sha1.hpp"
#include <string>
#include <vector>
#include <array>

namespace storrent
{
    struct info_hash
    {

        info_hash() {}
        info_hash(const info_hash&) = default;
        info_hash(info_hash&&) = default;

        explicit info_hash(const ::std::string& info_str)
        {
            _sha1.update(info_str);
            _hex_str = _sha1.final();
            for (size_t i = 0, j = 0; i < 5; i++, j += 4)
            {
                _bytes[j] = _sha1.raw_digest()[i] >> 24 & 0xFF;
                _bytes[j + 1] = _sha1.raw_digest()[i] >> 16 & 0xFF;
                _bytes[j + 2] = _sha1.raw_digest()[i] >> 8 & 0xFF;
                _bytes[j + 3] = _sha1.raw_digest()[i] & 0xFF;
            }
        }

        explicit info_hash(sha1 checksum) : _sha1{std::move(checksum)}
        {
            _hex_str = _sha1.final();
            for (size_t i = 0, j = 0; i < 5; i++, j += 4)
            {
                _bytes[j] = _sha1.raw_digest()[i] >> 24 & 0xFF;
                _bytes[j + 1] = _sha1.raw_digest()[i] >> 16 & 0xFF;
                _bytes[j + 2] = _sha1.raw_digest()[i] >> 8 & 0xFF;
                _bytes[j + 3] = _sha1.raw_digest()[i] & 0xFF;
            }
        }

        info_hash& operator=(const info_hash&) = default;
        info_hash& operator=(info_hash&&) = default;

        const std::string& hex() const { return _hex_str; }

        std::vector<char> bytes_vec() const { return {_bytes.begin(), _bytes.end()}; }

        const std::array<char, 20>& bytes() const { return _bytes; }

    private:
        std::array<char, 20> _bytes{};
        std::string _hex_str{};
        sha1 _sha1;
    };
}
