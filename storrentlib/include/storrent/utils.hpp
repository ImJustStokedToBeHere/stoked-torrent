#pragma once
#include "storrent/os_inc.hpp"
#include <boost/functional/hash.hpp>
#include <algorithm>
#include <array>
#include <charconv>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <string_view>

namespace storrent::utils
{
    bool file_exists(const std::string& aFileName);

    bool file_exists(std::string_view view);

    std::string read_file(const std::string& filename);

#if defined(_MSC_VER)
    std::wstring str_to_wstr(const std::string& s);

    std::string wstr_to_str(const std::wstring& s);
#endif

    std::string const& to_string(const std::string& s);

    std::wstring const& to_wstring(const std::wstring& s);

    template <typename... Args>
    std::string make_str(Args const&... args)
    {
        using std::to_string;
        using utils::to_string;

        std::string result;
        int unpack[]{0, (result += to_string(args), 0)...};
        static_cast<void>(unpack);
        return result;
    }

    template <typename... Args>
    [[maybe_unused]] std::wstring make_wstr(Args const&... args)
    {
        using std::to_wstring;
        using utils::to_wstring;

        std::wstring result;
        int unpack[]{0, (result += to_wstring(args), 0)...};
        static_cast<void>(unpack);
        return result;
    }

    template <typename... Args>
    void report_error(Args const&... args)
    {
        using std::to_string;
        using utils::to_string;

        std::string result;
        int unpack[]{0, (result += to_string(args), 0)...};
        static_cast<void>(unpack);

        std::cout << "ERROR: " << result << std::endl;
    }

    template <typename T>
    void hash_combine(std::size_t& seed, const T& val)
    {
        boost::hash_combine(seed, val);
    }

    std::chrono::milliseconds chrono_timestamp();

#ifdef UNICODE
    #define timestamp_str timestamp_str_w
    #define timestamp_str_fmt timestamp_str_fmt_w
    #define get_ip_from_hostname get_ip_from_hostname_w
#else
    #define timestamp_str timestamp_str_a
    #define timestamp_str_fmt timestamp_str_fmt_a
    #define get_ip_from_hostname get_ip_from_hostname_a
#endif

    std::wstring simple_str_to_wstr(const std::string& str);
    std::string simple_wstr_to_str(const std::wstring& str);
    //std::string ip_str(in_addr addr);
    std::string timestamp_str_a();
    std::wstring timestamp_str_w();
    time_t timestamp();
    time_t timestamp_add_seconds(int32_t val, time_t time = 0);
    time_t timestamp_add_minutes(int32_t val, time_t time = 0);
    time_t timestamp_add_hours(int32_t val, time_t time = 0);
    time_t timestamp_add_days(int32_t val, time_t time = 0);
    std::string timestamp_str_fmt_a(const std::string& aFmt, const bool& aAppendMilliseconds = false);
    std::wstring timestamp_str_fmt_w(const std::wstring& aFmt, const bool& aAppendMilliseconds = false);
    std::string timestamp_str_fmt_a(const std::string& aFmt, time_t val = 0);
    std::wstring timestamp_str_fmt_w(const std::wstring& aFmt, time_t val = 0);
    char* reverse_bytes(std::string val);
    char* reverse_bytes(char* start, size_t size);

    template <class IntType>
    IntType mersenne_twister_random(IntType a, IntType b)
    {
        static std::random_device rd;  // Will be used to obtain a seed for the random number engine
        static std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
        std::uniform_int_distribution<IntType> dis(a, b);
        return dis(gen);
    }

    template <class IntType>
    IntType default_random(IntType aStartRange, IntType aEndRange)
    {
        std::random_device random_device;
        std::default_random_engine random_engine(random_device());
        std::uniform_int_distribution<IntType> uniform_dist(aStartRange, aEndRange);
        return uniform_dist(random_engine);
    }

    /*std::string const& to_string(const std::string& s) { return s; }

    std::wstring const& to_wstring(const std::wstring& s) { return s; }

    template <typename... Args>
    std::string make_str(Args const&... args)
    {
        using std::to_string;
        using utils::to_string;

        std::string result;
        int unpack[]{0, (result += to_string(args), 0)...};
        static_cast<void>(unpack);
        return result;
    }

    template <typename... Args>
    std::wstring make_wstr(Args const&... args)
    {
        using std::to_wstring;
        using utils::to_wstring;

        std::wstring result;
        int unpack[]{0, (result += to_wstring(args), 0)...};
        static_cast<void>(unpack);
        return result;
    }*/

#ifdef UNICODE
    #define get_ip_from_hostname get_ip_from_hostname_w
#else
    #define get_ip_from_hostname get_ip_from_hostname_a
#endif

    //bool get_ip_from_hostname_w(const std::wstring& hostname,
    //                            const std::wstring& port_str,
    //                            IPPROTO protocol_designation,
    //                            std::wstring& ip,
    //                            SOCKADDR_IN& addr);

    //bool get_ip_from_hostname_a(const std::string& hostname,
    //                            const std::string& port_str,
    //                            IPPROTO protocol_designation,
    //                            std::string& ip,
    //                            SOCKADDR_IN& addr);

    bool wstr_to_int8(const std::wstring& content, int8_t& val);

    bool wstr_to_int16(const std::wstring& content, int16_t& val);

    bool wstr_to_int32(const std::wstring& content, int32_t& val);

    bool wstr_to_int64(const std::wstring& content, int64_t& val);

    bool wstr_to_uint8(const std::wstring& content, uint8_t& val);

    bool wstr_to_uint16(const std::wstring& content, uint16_t& val);

    bool wstr_to_uint32(const std::wstring& content, uint32_t& val);

    bool wstr_to_uint64(const std::wstring& content, uint64_t& val);

    bool str_to_int8(const std::string& content, int8_t& val);

    bool str_to_int16(const std::string& content, int16_t& val);

    bool str_to_int32(const std::string& content, int32_t& val);

    bool str_to_int64(const std::string& content, int64_t& val);

    bool str_to_uint8(const std::string& content, uint8_t& val);

    bool str_to_uint16(const std::string& content, uint16_t& val);

    bool str_to_uint32(const std::string& content, uint32_t& val);

    bool str_to_uint64(const std::string& content, uint64_t& val);

    size_t wildcard_pos(const std::string& aText,
                        const std::string& aSubStr,
                        size_t aIndex,
                        const bool& aIgnoreCase,
                        const bool& aWildCard);

    size_t replace_substr(std::string& text,
                          const std::string& old_substr,
                          const std::string& new_substr,
                          const bool& ignore_case,
                          const bool& use_wildcard);

    size_t generate_random_byte_str(char* aBuffer, size_t aIndex, size_t aCount);

    //int last_socket_err();

    //std::string socket_err_str(int err);

    //std::string last_socket_err_str();
} // namespace storrent::utils
