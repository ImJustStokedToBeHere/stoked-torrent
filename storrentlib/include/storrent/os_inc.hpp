#pragma once
#if defined(_MSC_VER) || defined(WIN_VER)
    #include <SDKDDKVer.h>
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #include <WinSock2.h>
    #include <ws2tcpip.h>
#else
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <sys/time.h>

    #ifndef OutputDebugString
        #include <iostream>
        #define OutputDebugString(str) std::cout << str << std::endl;
    #endif
#endif