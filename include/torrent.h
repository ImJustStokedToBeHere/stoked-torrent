#pragma once
#include <utils.h>
#include <string>
#include <vector>
#include <memory>

namespace stoked {

    struct torrent_file {
    public:
        torrent_file(std::vector<std::string> path, size_t length)
            : path{std::move(path)},
            length{length} { }

        torrent_file(const torrent_file&) = default;
        torrent_file(torrent_file&&) = default;

        torrent_file& operator =(const torrent_file&) = default;
        torrent_file& operator =(torrent_file&&) = default;

        ~torrent_file() = default;

        const std::vector<std::string> path;
        const size_t length;
    };

    struct torrent {
    public:
        // int v = sizeof(torrent_file);
        static std::shared_ptr<torrent> load_torrent_file(const std::string& filename);

        torrent() : torrent({}, {}, {}, {}, 0, {}, {}, 0, {}, true, {}, {}) {
        }

        torrent(std::string filename, std::string name,
                std::vector<torrent_file> torrent_files,
                std::string pieces, size_t piece_len,
                std::string comment, std::string created_by,
                size_t creation_date, std::vector<std::string> announce_list,
                bool single_file, std::string info_hash, std::vector<char> info_hash_bytes)
            : _filename{std::move(filename)},
            _name{std::move(name)},
            _files{std::move(torrent_files)},
            _pieces{std::move(pieces)},
            _piece_len{piece_len},
            _comment{std::move(comment)},
            _created_by{std::move(created_by)},
            _creation_date{creation_date},
            _announce_list{std::move(announce_list)},
            _single_file{single_file},
            _info_hash{info_hash},
            _info_hash_bytes{info_hash_bytes} {
            /*noop*/
        }

        torrent(const torrent&) = delete;
        torrent(torrent&&) = default;

        std::string filename() const {
            return _filename;
        }

        std::string name() const {
            return _name;
        }

        std::string pieces() const {
            return _pieces;
        }

        std::string comment() const {
            return _comment;
        }

        std::string created_by() const {
            return _created_by;
        }

        size_t piece_len() const {
            return _piece_len;
        }

        size_t creation_date() const {
            return _creation_date;
        }

        bool is_single_file() const {
            return _single_file;
        }


#ifdef UNICODE
#define info_hash_bytes_hex info_hash_bytes_hex_w
#else
#define info_hash_bytes_hex info_hash_bytes_hex_a
#endif

        const std::string& info_hash_bytes_hex_a() const {
            return _info_hash;
        }

        std::wstring info_hash_bytes_hex_w() const {
            return stoked::utils::str_to_wstr(info_hash_bytes_hex_a());
        }

        const std::vector<char>& info_hash_bytes() const {
            return _info_hash_bytes;
        }

        const std::vector<std::string>& announce_list() const {
            return _announce_list;
        }

        const std::vector<torrent_file>& torrent_files() const {
            return _files;
        }

        size_t byte_len() const {

        }


    private:
        // int g = sizeof(std::vector<char>);
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
        std::string _info_hash;
        std::vector<char> _info_hash_bytes;
    };

}