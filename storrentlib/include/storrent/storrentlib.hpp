#pragma once
#include "storrent/os_inc.hpp"
#include "storrent/info_hash.hpp"
#include <string>

namespace storrent
{
    info_hash start_session(const std::string& filename, const std::string& dir_name);
    typedef size_t download_progress;
    info_hash get_download_progress(int download_id, download_progress& progress);
}