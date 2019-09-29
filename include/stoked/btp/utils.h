#pragma once
#include <stoked/btp/os_includes.h>
#include <cstdint>
#include <ctime>
#include <chrono>
#include <string>
#include <random>
#include <algorithm>
#include <charconv>
#include <array>


namespace stoked::btp::utils {
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

    inline std::wstring simple_str_to_wstr(const std::string& str) {
        return {str.cbegin(), str.cend()};
    }

    inline std::string simple_wstr_to_str(const std::wstring& str) {
        return {str.cbegin(), str.cend()};
    }

    inline std::wstring str_to_wstr(const std::string& s) {
        size_t slength = s.length() + 1;
        size_t len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
        std::wstring r(len, L'\0');
        MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, &r[0], len);
        return r;
    }

    inline std::string wstr_to_str(const std::wstring& s) {
        size_t slength = s.length() + 1;
        size_t len = WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0);
        std::string r(len, '\0');
        WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, &r[0], len, 0, 0);
        return r;
    }

    inline std::string ip_str(in_addr addr) {
        char ip[16];
        inet_ntop(AF_INET, &addr, ip, 16);
        return std::string{ip};
    }

    std::string timestamp_str_a();
    std::wstring timestamp_str_w();
    time_t timestamp();

    inline time_t timestamp_add_seconds(int32_t val, time_t time = 0) {
        time_t now{time};
        if (!now) {
            now = timestamp();
        }

        tm timeinfo{};
        localtime_s(&timeinfo, &now);

        int years = val / (60 * 60 * 24 * 365);
        val -= years * (60 * 60 * 24 * 365);
        int days = val / (60 * 60 * 24);
        val -= days * (60 * 60 * 24);
        int hours = val / (60 * 60);
        val -= hours * (60 * 60);
        auto min = val / 60;
        val -= min * 60;

        timeinfo.tm_year += years;
        timeinfo.tm_yday += days;
        timeinfo.tm_hour += hours;
        timeinfo.tm_min += min;
        timeinfo.tm_sec += val;

        return mktime(&timeinfo);
    }

    inline time_t timestamp_add_minutes(int32_t val, time_t time = 0) {
        time_t now{time};
        if (!now) {
            now = timestamp();
        }

        tm timeinfo{};
        localtime_s(&timeinfo, &now);

        int years = val / (60 * 60 * 24 * 365);
        val -= years * (60 * 60 * 24 * 365);
        int days = val / (60 * 60 * 24);
        val -= days * (60 * 60 * 24);
        int hours = val / (60 * 60);
        val -= hours * (60 * 60);

        timeinfo.tm_year += years;
        timeinfo.tm_yday += days;
        timeinfo.tm_hour += hours;
        timeinfo.tm_min += val;
        return mktime(&timeinfo);
    }

    inline time_t timestamp_add_hours(int32_t val, time_t time = 0) {
        time_t now{time};
        if (!now) {
            now = timestamp();
        }

        tm timeinfo{};
        localtime_s(&timeinfo, &now);

        int years = val / (60 * 60 * 24 * 365);
        val -= years * (60 * 60 * 24 * 365);
        int days = val / (60 * 60 * 24);
        val -= days * (60 * 60 * 24);

        timeinfo.tm_year += int(years);
        timeinfo.tm_yday += int(days);
        timeinfo.tm_hour += int(val);
        return mktime(&timeinfo);
    }

    inline time_t timestamp_add_days(int32_t val, time_t time = 0) {
        time_t now{time};
        if (!now) {
            now = timestamp();
        }

        tm timeinfo{};
        localtime_s(&timeinfo, &now);
        int years = val / (60 * 60 * 24 * 365);
        val -= years * (60 * 60 * 24 * 365);

        timeinfo.tm_year += years;
        timeinfo.tm_yday += val;
        return mktime(&timeinfo);
    }

    std::string timestamp_str_fmt_a(const std::string& aFmt, const bool& aAppendMilliseconds = false);
    std::wstring timestamp_str_fmt_w(const std::wstring& aFmt, const bool& aAppendMilliseconds = false);
    std::string timestamp_str_fmt_a(const std::string& aFmt, time_t val = 0);
    std::wstring timestamp_str_fmt_w(const std::wstring& aFmt, time_t val = 0);
    char* reverse_bytes(std::string val);
    char* reverse_bytes(char* start, size_t size);

    inline char* reverse_bytes(std::string val) {
        return reverse_bytes(val.data(), val.size());
    }

    inline char* reverse_bytes(char* start, size_t size) {
        char* s = start;
        char* e = s + size;

        std::reverse(s, e);
        return s;
    }

    template <class IntType>
    IntType mersenne_twister_random(IntType a, IntType b) {
        static std::random_device rd;  //Will be used to obtain a seed for the random number engine
        static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
        std::uniform_int_distribution<IntType> dis(a, b);
        return dis(gen);
    }

    template<class IntType>
    IntType default_random(IntType aStartRange, IntType aEndRange) {
        std::random_device random_device;
        std::default_random_engine random_engine(random_device());
        std::uniform_int_distribution<IntType> uniform_dist(aStartRange, aEndRange);
        return uniform_dist(random_engine);
    }

    inline std::string const& to_string(const std::string& s) {
        return s;
    }

    inline std::wstring const& to_wstring(const std::wstring& s) {
        return s;
    }

    template<typename... Args>
    std::string make_str(Args const&... args) {
        using utils::to_string;
        using std::to_string;

        std::string result;
        int unpack[]{0, (result += to_string(args), 0)...};
        static_cast<void>(unpack);
        return result;
    }

    template<typename... Args>
    std::wstring make_wstr(Args const&... args) {
        using utils::to_wstring;
        using std::to_wstring;

        std::wstring result;
        int unpack[]{0, (result += to_wstring(args), 0)...};
        static_cast<void>(unpack);
        return result;
    }

#ifdef UNICODE
#define get_ip_from_hostname get_ip_from_hostname_w
#else
#define get_ip_from_hostname get_ip_from_hostname_a
#endif

    inline bool get_ip_from_hostname_w(const std::wstring& hostname, const std::wstring& port_str, IPPROTO protocol_designation, std::wstring& ip, SOCKADDR_IN& addr) {
                
        if (WSAData wsa{}; 0 != WSAStartup(MAKEWORD(2, 2), &wsa)) {
            throw WSAGetLastError();
        }

        ADDRINFOW* result = NULL;
        ADDRINFOW hints{};

        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_INET;
        // hints.ai_socktype = SOCK_DGRAM;		
        hints.ai_protocol = protocol_designation;

        DWORD host_type_namespace = NS_ALL;
        auto ret = GetAddrInfoW(hostname.c_str(), port_str.c_str(), &hints, &result);
        if (ret) {
            return false;
        }

        sockaddr_in ipv4 = *((struct sockaddr_in*) result->ai_addr);
        wchar_t ipstringbuffer[46];
        InetNtopW(AF_INET, &ipv4.sin_addr, ipstringbuffer, 46);

        ip = ipstringbuffer;
        addr = ipv4;
        WSACleanup();
        return true;
    }

    inline bool get_ip_from_hostname_a(const std::string& hostname, const std::string& port_str, IPPROTO protocol_designation, std::string& ip, SOCKADDR_IN& addr) {
        if (WSAData wsa{}; 0 != WSAStartup(MAKEWORD(2, 2), &wsa)) {
            throw WSAGetLastError();
        }

        ADDRINFOA* result = NULL;
        ADDRINFOA hints{};

        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_INET;
        // hints.ai_socktype = SOCK_DGRAM;		
        hints.ai_protocol = protocol_designation;

        DWORD host_type_namespace = NS_ALL;
        auto ret = GetAddrInfoA(hostname.c_str(), port_str.c_str(), &hints, &result);
        if (ret) {
            return false;
        }

        sockaddr_in ipv4 = *((struct sockaddr_in*) result->ai_addr);
        char ipstringbuffer[46];
        InetNtopA(AF_INET, &ipv4.sin_addr, ipstringbuffer, 46);

        ip = ipstringbuffer;
        addr = ipv4;
        WSACleanup();
        return true;
    }

    inline bool wstr_to_int8(const std::wstring& content, int8_t& val) {
        int8_t result{};
        std::string str{wstr_to_str(content)};

        if (auto parsed_result = std::from_chars(str.c_str(), str.c_str() + str.length() - 1, result); parsed_result.ec == std::errc()) {
            val = result;
            return true;
        } else {
            return false;
        }
    }

    inline bool wstr_to_int16(const std::wstring& content, int16_t& val) {
        int16_t result{};
        std::string str{wstr_to_str(content)};

        if (auto parsed_result = std::from_chars(str.c_str(), str.c_str() + str.length() - 1, result); parsed_result.ec == std::errc()) {
            val = result;
            return true;
        } else {
            return false;
        }
    }

    inline bool wstr_to_int32(const std::wstring& content, int32_t& val) {
        int32_t result{};
        std::string str{wstr_to_str(content)};

        if (auto parsed_result = std::from_chars(str.c_str(), str.c_str() + str.length() - 1, result); parsed_result.ec == std::errc()) {
            val = result;
            return true;
        } else {
            return false;
        }
    }

    inline bool wstr_to_int64(const std::wstring& content, int64_t& val) {
        int64_t result{};
        std::string str{wstr_to_str(content)};

        if (auto parsed_result = std::from_chars(str.c_str(), str.c_str() + str.length() - 1, result); parsed_result.ec == std::errc()) {
            val = result;
            return true;
        } else {
            return false;
        }
    }


    inline bool wstr_to_uint8(const std::wstring& content, uint8_t& val) {
        uint8_t result{};
        std::string str{wstr_to_str(content)};

        if (auto parsed_result = std::from_chars(str.c_str(), str.c_str() + str.length() - 1, result); parsed_result.ec == std::errc()) {
            val = result;
            return true;
        } else {
            return false;
        }
    }

    inline bool wstr_to_uint16(const std::wstring& content, uint16_t& val) {
        uint16_t result{};
        std::string str{wstr_to_str(content)};

        if (auto parsed_result = std::from_chars(str.c_str(), str.c_str() + str.length() - 1, result); parsed_result.ec == std::errc()) {
            val = result;
            return true;
        } else {
            return false;
        }
    }

    inline bool wstr_to_uint32(const std::wstring& content, uint32_t& val) {
        uint32_t result{};
        std::string str{wstr_to_str(content)};

        if (auto parsed_result = std::from_chars(str.c_str(), str.c_str() + str.length() - 1, result); parsed_result.ec == std::errc()) {
            val = result;
            return true;
        } else {
            return false;
        }
    }

    inline bool wstr_to_uint64(const std::wstring& content, uint64_t& val) {
        uint64_t result{};
        std::string str{wstr_to_str(content)};

        if (auto parsed_result = std::from_chars(str.c_str(), str.c_str() + str.length() - 1, result); parsed_result.ec == std::errc()) {
            val = result;
            return true;
        } else {
            return false;
        }
    }

    inline bool str_to_int8(const std::string& content, int8_t& val) {
        int8_t result{};
        std::string str{(content)};

        if (auto parsed_result = std::from_chars(str.c_str(), str.c_str() + str.length() - 1, result); parsed_result.ec == std::errc()) {
            val = result;
            return true;
        } else {
            return false;
        }
    }

    inline bool str_to_int16(const std::string& content, int16_t& val) {
        int16_t result{};
        std::string str{(content)};

        if (auto parsed_result = std::from_chars(str.c_str(), str.c_str() + str.length() - 1, result); parsed_result.ec == std::errc()) {
            val = result;
            return true;
        } else {
            return false;
        }
    }

    inline bool str_to_int32(const std::string& content, int32_t& val) {
        int32_t result{};
        std::string str{(content)};

        if (auto parsed_result = std::from_chars(str.c_str(), str.c_str() + str.length() - 1, result); parsed_result.ec == std::errc()) {
            val = result;
            return true;
        } else {
            return false;
        }
    }

    inline bool str_to_int64(const std::string& content, int64_t& val) {
        int64_t result{};
        std::string str{(content)};

        if (auto parsed_result = std::from_chars(str.c_str(), str.c_str() + str.length() - 1, result); parsed_result.ec == std::errc()) {
            val = result;
            return true;
        } else {
            return false;
        }
    }


    inline bool str_to_uint8(const std::string& content, uint8_t& val) {
        uint8_t result{};
        std::string str{(content)};

        if (auto parsed_result = std::from_chars(str.c_str(), str.c_str() + str.length() - 1, result); parsed_result.ec == std::errc()) {
            val = result;
            return true;
        } else {
            return false;
        }
    }

    inline bool str_to_uint16(const std::string& content, uint16_t& val) {
        uint16_t result{};
        std::string str{(content)};

        if (auto parsed_result = std::from_chars(str.c_str(), str.c_str() + str.length(), result); parsed_result.ec == std::errc()) {
            val = result;
            return true;
        } else {
            return false;
        }
    }

    inline bool str_to_uint32(const std::string& content, uint32_t& val) {
        uint32_t result{};
        std::string str{(content)};

        if (auto parsed_result = std::from_chars(str.c_str(), str.c_str() + str.length() - 1, result); parsed_result.ec == std::errc()) {
            val = result;
            return true;
        } else {
            return false;
        }
    }

    inline bool str_to_uint64(const std::string& content, uint64_t& val) {
        uint64_t result{};
        std::string str{(content)};

        if (auto parsed_result = std::from_chars(str.c_str(), str.c_str() + str.length() - 1, result); parsed_result.ec == std::errc()) {
            val = result;
            return true;
        } else {
            return false;
        }
    }

    inline size_t wildcard_pos(const std::string& aText, const std::string& aSubStr, size_t aIndex, const bool& aIgnoreCase, const bool& aWildCard) {
        size_t zResult = std::string::npos;
        size_t zLenSub = (int)aSubStr.size();
        size_t zLenTxt = (int)aText.size();

        if (zLenSub < 1)
            return zResult;
        if (zLenTxt < 1)
            return zResult;
        if (zLenTxt < aIndex)
            return zResult;

        size_t zSearchIdx = 0;
        size_t zSubStrIdx = 0;

        if (aIndex > 0)
            zSearchIdx = (int)aIndex;

        size_t zSetIdx = zSearchIdx;

        do {
            char zChar = aText[zSearchIdx];
            char zFindChar = aSubStr[zSubStrIdx];

            if (zChar == zFindChar
                || (aIgnoreCase && (toupper(zChar) == toupper(zFindChar)))
                || (aWildCard && (zFindChar == '?'))) {
                if (zSubStrIdx == zLenSub - 1) {
                    zResult = zSetIdx;
                    break;
                }

                zSearchIdx++;
                zSubStrIdx++;

            } else {
                zSetIdx++;
                zSearchIdx = zSetIdx;
                zSubStrIdx = 0;
            }
        } while (zSearchIdx < zLenTxt);

        return zResult;
    }


    inline size_t replace_substr(std::string& text, const std::string& old_substr, const std::string& new_substr, const bool& ignore_case, const bool& use_wildcard) {

        size_t len_old_substr{old_substr.size()};
        if (len_old_substr == 0)
            return 0; // nothing to replace, save us some work

        size_t found_count{0};
        size_t search_idx{0};
        std::array<size_t, 256> found_list{};

        do {
            search_idx = wildcard_pos(text, old_substr, search_idx, ignore_case, use_wildcard);
            if (search_idx != std::string::npos) {
                if (found_count > 255)
                    throw std::exception("Stoked::Utils::ReplaceText cannot do more than 256 replacements");

                found_list[found_count] = search_idx;

                found_count++;
                search_idx += len_old_substr;
            } else {
                break;
            }
        } while (true);


        if (found_count > 0) {
            size_t len_new_substr = new_substr.size();
            size_t buf_size = text.size() + (found_count * (len_new_substr - len_old_substr));
            std::unique_ptr<char[]> buffer(new char[buf_size + 1]);
            // std::vector<char> buffer(buf_size + 1);

            size_t write_idx{0};
            size_t found_itr{0};
            size_t len_before{0};
            search_idx = 0;

            while (found_itr < found_count) {
                len_before = found_list[found_itr] - search_idx;
                memmove(&buffer.get()[write_idx], &text.c_str()[search_idx], len_before * sizeof(char)); // possibly copies 0 chars
                write_idx += len_before;

                if (len_new_substr > 0) {
                    memmove(&buffer.get()[write_idx], &new_substr.c_str()[0], len_new_substr * sizeof(char));
                    write_idx += len_new_substr;
                }

                search_idx += len_before;
                search_idx += len_old_substr;
                found_itr++;
            }

            len_before = text.size() - search_idx;
            if (len_before > 0)
                memmove(&buffer.get()[write_idx], &text.c_str()[search_idx], len_before * sizeof(char));

            buffer.get()[buf_size] = 0;
            text = buffer.get();
            // delete[] buffer;
        }

        return found_count;
    }

    inline size_t generate_random_byte_str(char* aBuffer, size_t aIndex, size_t aCount) {
        // const static char * LOOKUP = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        const static std::string LOOKUP = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        size_t zI = 0;

        while (zI < aCount) {
            int zRand = default_random(0, 127);
            char zChar = static_cast<char>(zRand);
            std::string zCharStr(1, zChar);
            if (wildcard_pos(LOOKUP, zCharStr, 0, false, false) >= 0) {
                if (aBuffer + zI + aIndex != NULL) {
                    *(aBuffer + zI + aIndex) = zChar;
                    zI++;
                } else {
                    break;
                }
            }
        }

        return zI;
    }

    inline int last_socket_err() {
#if defined(_MSC_VER) || defined(WIN_VER)
        return WSAGetLastError();
#else
        return 0;
#endif
    }

    inline std::string socket_err_str(int err) {
#if defined(_MSC_VER) || defined(WIN_VER)
        char* s = NULL;
        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL, err,
                       MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                       (LPSTR)&s, 0, NULL);
        // fprintf(stderr, "%S\n", s);
        std::string result{s};
        LocalFree(s);

        return result;
#else
        return { };
#endif
    }

    inline std::string last_socket_err_str() {
        return socket_err_str(WSAGetLastError());
    }
}
