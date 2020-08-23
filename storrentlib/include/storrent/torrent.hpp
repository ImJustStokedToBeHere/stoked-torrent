#pragma once
#include "storrent/info_hash.hpp"
#include "storrent/torrent_file.hpp"
#include <cmath>
#include <memory>
#include <string>
#include <vector>
#include <cmath>

namespace storrent
{
    struct torrent;
    typedef std::shared_ptr<torrent> torrent_handle;

    struct torrent
    {
    public:
        static constexpr auto BLOCK_LEN = 0x4000;
        static torrent_handle load_torrent_file(const std::string& filename);

        torrent() : torrent({}, {}, {}, {}, 0, {}, {}, 0, {}, {}, true, {}) {}

        torrent(std::string filename,
                std::string name,
                std::vector<torrent_file> torrent_files,
                std::string pieces,
                size_t piece_len,
                std::string comment,
                std::string created_by,
                size_t creation_date,
                std::string announce,
                std::vector<std::string> announce_list,
                bool single_file,
                info_hash hash)
            : _filename{std::move(filename)},
              _name{std::move(name)},
              _files{std::move(torrent_files)},
              _pieces{std::move(pieces)},
              _piece_len{piece_len},
              _comment{std::move(comment)},
              _created_by{std::move(created_by)},
              _creation_date{creation_date},
              _announce{announce},
              _announce_list{std::move(announce_list)},
              _single_file{single_file},
              _info_hash{std::move(hash)}
        {
            /* noop */
        }

        torrent(const torrent&) = delete;
        torrent(torrent&&) = default;

        std::string filename() const { return _filename; }

        std::string name() const { return _name; }

        std::string pieces() const { return _pieces; }

        std::string comment() const { return _comment; }

        std::string created_by() const { return _created_by; }

        size_t piece_len() const { return _piece_len; }

        size_t creation_date() const { return _creation_date; }

        bool is_single_file() const { return _single_file; }

        size_t blocks_per_pc(size_t pc_idx) const
        {
            const auto pc_len = piece_len(pc_idx);
            return static_cast<size_t>(std::ceil(pc_len / BLOCK_LEN));
        }

        size_t piece_len(size_t pc_idx) const
        {
            const size_t total = size();
            const size_t pc_len = piece_len();
            const auto last_pc_len = total % pc_len;
            const size_t last_pc_idx = static_cast<size_t>(floor(total / pc_len));
            return last_pc_idx == pc_idx ? last_pc_len : pc_len;
        }

        std::string announce() const { return _announce; }
        const std::vector<std::string>& announce_list() const { return _announce_list; }

        const std::vector<torrent_file>& torrent_files() const { return _files; }

        size_t byte_len() const {}

        const info_hash& info_hash_val() const { return _info_hash; }

        size_t size() const
        {
            size_t len = 0;
            for (auto& tf : _files)
                len += tf.length;
            return len;
        }

    private:
        std::string _announce;
        std::string _filename;
        std::string _name;
        std::vector<torrent_file> _files;
        std::string _pieces;
        size_t _piece_len;
        std::string _comment;
        std::string _created_by;
        uintptr_t _creation_date;
        std::vector<std::string> _announce_list;
        bool _single_file;
        info_hash _info_hash;
    };

} // namespace storrent
