#include <stoked/btp/stoked_torrent.h>
#include <stoked/btp/os_includes.h>
#include <stoked/btp/utils.h>
#include <map>

namespace stoked::btp {

    namespace {

        int _storrent_state = STORRENT_UNINITIALIZED;

        std::map<int, const char*> _err_messages{
            {STORRENT_ERR_WSA_FAILED, "wsa startup failed"}
        };


        SOCKET _socket{};
        SOCKADDR_IN _addr{};
    }

    int setup_stoked_torrent() {
        if (_storrent_state != STORRENT_UNINITIALIZED)
            return _storrent_state;

#if defined(_MSC_VER) || defined(WIN_VER)
        WSADATA wsa{};
        if (auto wsa_ret = WSAStartup(MAKEWORD(2, 2), &wsa); wsa_ret != 0) {
            return _storrent_state = STORRENT_ERR_WSA_FAILED;
        }
        OutputDebugString(stoked::btp::utils::make_str("stoked torrent started!\n").c_str());
#else
#endif

        return _storrent_state = STORRENT_INITIALIZED;
    }

    const char* err_str_from_code(int err_code) {
        return _err_messages[err_code];
    }



    int teardown_stoked_torrent() {
#if defined(_MSC_VER) || defined(WIN_VER)
        WSACleanup();
        OutputDebugString(stoked::btp::utils::make_str("stoked torrent shutdown!\n").c_str());
#else
#endif
        return STORRENT_UNINITIALIZED;
    }
}