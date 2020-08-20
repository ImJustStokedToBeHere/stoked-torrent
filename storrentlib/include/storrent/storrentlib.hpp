#pragma once
//#include "storrent/winver.hpp"
#include "storrent/os_inc.hpp"
#include <string>

namespace storrent
{
    typedef int session_id;
    constexpr auto INVALID_SESSION_ID = -1;
    session_id download_file(const std::string& filename);
    typedef size_t download_progress;
    session_id get_download_progress(session_id download_id, download_progress& progress);
}