#pragma once
#include <vector>
#include <string>

namespace storrent
{
    struct torrent_file
    {
    public:
        torrent_file(std::vector<std::string> path, size_t length) : path{std::move(path)}, length{length} {}

        torrent_file(const torrent_file&) = default;
        torrent_file(torrent_file&&) = default;

        torrent_file& operator=(const torrent_file&) = default;
        torrent_file& operator=(torrent_file&&) = default;

        ~torrent_file() = default;

        const std::vector<std::string> path;
        const size_t length;
    };
}
