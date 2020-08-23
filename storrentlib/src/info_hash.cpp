#include "storrent/info_hash.hpp"

namespace storrent
{
    info_hash::info_hash(const ::std::string& info_str)
    {
        sha1 hasher;
        hasher.update(info_str);
        _hex_str = hasher.final();
        for (size_t i = 0, j = 0; i < 5; i++, j += 4)
        {
            _bytes[j] = hasher.raw_digest()[i] >> 24 & 0xFF;
            _bytes[j + 1] = hasher.raw_digest()[i] >> 16 & 0xFF;
            _bytes[j + 2] = hasher.raw_digest()[i] >> 8 & 0xFF;
            _bytes[j + 3] = hasher.raw_digest()[i] & 0xFF;
        }
    }

    info_hash::info_hash(sha1 checksum) // : _sha1{std::move(checksum)}
    {
        _hex_str = checksum.final();
        for (size_t i = 0, j = 0; i < 5; i++, j += 4)
        {
            _bytes[j] = checksum.raw_digest()[i] >> 24 & 0xFF;
            _bytes[j + 1] = checksum.raw_digest()[i] >> 16 & 0xFF;
            _bytes[j + 2] = checksum.raw_digest()[i] >> 8 & 0xFF;
            _bytes[j + 3] = checksum.raw_digest()[i] & 0xFF;
        }
    }

    bool operator==(const info_hash& lhs, const info_hash& rhs)
    {
        /*std::hash<info_hash> this_hash;
        std::hash<info_hash> other_hash;*/

        // return this_hash(lhs) == other_hash(rhs);
        // return std::hash<std::string>{lhs.hex()}() == std::hash<std::string>{rhs.hex()}();
        return lhs.hex() == rhs.hex();
    }

    bool operator!=(const info_hash& lhs, const info_hash& rhs) { return !operator==(lhs, rhs); }

} // namespace storrent