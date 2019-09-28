#pragma once
#include <string>

namespace stoked {

    inline const int STORRENT_UNINITIALIZED = 0;
    inline const int STORRENT_INITIALIZED = 1;
    inline const int STORRENT_ERR_WSA_FAILED = 2;

    int setup_stoked_torrent();
    const char* err_str_from_code(int err_code);

    int teardown_stoked_torrent();
}
