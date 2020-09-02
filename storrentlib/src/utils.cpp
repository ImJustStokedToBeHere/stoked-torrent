#include "storrent/utils.hpp"
#include <codecvt>
#include <locale>

namespace storrent::utils
{
    bool file_exists(const std::string& aFileName)
    {
        std::ifstream file(aFileName);
        return (bool)file;
    }

    bool file_exists(std::string_view view)
    {
        std::ifstream file(view.data());
        return bool{file};
    }

    std::string read_file(const std::string& filename)
    {
        std::ifstream file(filename);
        return file ? std::string((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()))
                    : std::string();
    }

#if defined(_MSC_VER) || defined(WIN_VER)
    std::wstring str_to_wstr(const std::string& s)
    {
        auto slength = static_cast<int>(s.length() + 1);
        auto len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), static_cast<int>(slength), 0, 0);
        std::wstring r(len, L'\0');
        MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, &r[0], len);
        return r;
    }

    std::string wstr_to_str(const std::wstring& s)
    {
        auto slength = static_cast<int>(s.length() + 1);
        auto len = WideCharToMultiByte(CP_ACP, 0, s.c_str(), static_cast<int>(slength), 0, 0, 0, 0);
        std::string r(len, '\0');
        WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, &r[0], len, 0, 0);
        return r;
    }
#else
    std::wstring str_to_wstr(const std::string& s) { return simple_str_to_wstr(s); }

    std::wstring wstr_to_str(const std::wstring& s) { return simple_wstr_to_str(s); }
#endif

    std::string const& to_string(const std::string& s) { return s; }

    std::wstring const& to_wstring(const std::wstring& s) { return s; }

    // std::string const& to_string(const std::string_view& s) { return s.data(); }

    std::chrono::milliseconds chrono_timestamp()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch());
    }

    std::wstring simple_str_to_wstr(const std::string& str)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        return converter.from_bytes(str);
    }

    std::string simple_wstr_to_str(const std::wstring& str)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        return converter.to_bytes(str);
    }

#if defined(_MSC_VER) || defined(WIN_VER)
    typedef struct timeval
    {
        long tv_sec;
        long tv_usec;
    } timeval;
#endif

#if defined(_MSC_VER) || defined(WIN_VER)
    int64_t time_of_day(timeval* tp)
    {
        // Note: some broken versions only have 8 trailing zero's, the correct epoch
        // has 9 trailing zero's This magic number is the number of 100 nanosecond
        // intervals since January 1, 1601 (UTC) until 00:00:00 January 1, 1970
        static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

        SYSTEMTIME system_time;
        FILETIME file_time;
        uint64_t time;

        GetSystemTime(&system_time);
        SystemTimeToFileTime(&system_time, &file_time);
        time = ((uint64_t)file_time.dwLowDateTime);
        time += ((uint64_t)file_time.dwHighDateTime) << 32;

        tp->tv_sec = (long)((time - EPOCH) / 10000000L);
        tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
        return 0;
    }

    std::wstring timestamp_str_w()
    {
        timeval curTime;
        time_of_day(&curTime);
        int milli = curTime.tv_usec / 1000;

        time_t rawtime = time(NULL);
        struct tm* timeinfo = new tm;
        // tm timeinfo(NULL);
        wchar_t buffer[80];

        time(&rawtime);
        localtime_s(timeinfo, &rawtime);

        wcsftime(buffer, 80, L"%Y-%m-%d %H:%M:%S", timeinfo);

        wchar_t currentTime[84] = L"";
        swprintf_s(currentTime, L"%s.%d", buffer, milli);

        return std::wstring(currentTime);
    }

    // std::string ip_str(in_addr addr)
    //{
    //    char ip[16];
    //    inet_ntop(AF_INET, &addr, ip, 16);
    //    return std::string{ip};
    //}

    std::string timestamp_str_a()
    {
        timeval curTime;
        time_of_day(&curTime);
        int milli = curTime.tv_usec / 1000;

        time_t rawtime = time(NULL);
        struct tm* timeinfo = new tm;
        // tm timeinfo(NULL);
        char buffer[80];

        time(&rawtime);
        localtime_s(timeinfo, &rawtime);

        strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);

        char currentTime[84] = "";
        sprintf_s(currentTime, "%s.%d", buffer, milli);

        return std::string(currentTime);
    }

    std::string timestamp_str_fmt_a(const std::string& aFmt, const bool& aAppendMilliseconds)
    {
        timeval curTime;
        time_of_day(&curTime);
        int milli = curTime.tv_usec / 1000;

        time_t rawtime;
        // struct tm *timeinfo = new tm;
        tm timeinfo{};
        // tm timeinfo;
        char buffer[80];

        time(&rawtime);
        localtime_s(&timeinfo, &rawtime);

        strftime(&buffer[0], 80, aFmt.c_str(), &timeinfo);

        char currentTime[84] = "";
        if (!aAppendMilliseconds)
        {
            sprintf_s(currentTime, "%s", buffer);
        }
        else
        {
            sprintf_s(currentTime, "%s.%d", buffer, milli);
        }

        return std::string(currentTime);
    }

    std::string timestamp_str_fmt_a(const std::string& aFmt, time_t val)
    {

        time_t rawtime{val};
        // struct tm *timeinfo = new tm;
        tm timeinfo{};
        // tm timeinfo;
        char buffer[80];
        if (!val)
            time(&rawtime);
        localtime_s(&timeinfo, &rawtime);
        strftime(&buffer[0], 80, aFmt.c_str(), &timeinfo);

        return std::string(buffer);
    }

    std::wstring timestamp_str_fmt_w(const std::wstring& aFmt, const bool& aAppendMilliseconds)
    {
        timeval curTime;
        time_of_day(&curTime);
        int milli = curTime.tv_usec / 1000;

        time_t rawtime;
        // struct tm *timeinfo = new tm;
        tm timeinfo{};
        // tm timeinfo;
        wchar_t buffer[80];

        time(&rawtime);
        localtime_s(&timeinfo, &rawtime);

        wcsftime(&buffer[0], 80, aFmt.c_str(), &timeinfo);

        wchar_t currentTime[84] = L"";
        if (!aAppendMilliseconds)
        {
            swprintf_s(currentTime, L"%s", buffer);
        }
        else
        {
            swprintf_s(currentTime, L"%s.%d", buffer, milli);
        }

        return std::wstring(currentTime);
    }

    std::wstring timestamp_str_fmt_w(const std::wstring& aFmt, time_t val)
    {

        time_t rawtime{val};
        // struct tm *timeinfo = new tm;
        tm timeinfo{};
        // tm timeinfo;
        wchar_t buffer[80];
        if (!val)
            time(&rawtime);
        localtime_s(&timeinfo, &rawtime);
        wcsftime(&buffer[0], 80, aFmt.c_str(), &timeinfo);

        return std::wstring(buffer);
    }

#else

    int64_t time_of_day(timeval* tp)
    {
        gettimeofday(tp, nullptr);                                         // get current time
        long long milliseconds = tp->tv_sec * 1000LL + tp->tv_usec / 1000; // calculate milliseconds
        // printf("milliseconds: %lld\n", milliseconds);
        return milliseconds;
    }

    std::string timestamp_str()
    {
        timeval curTime;
        time_of_day(&curTime);
        int milli = curTime.tv_usec / 1000;

        time_t rawtime = time(NULL);
        // tm timeinfo(NULL);
        char buffer[80];

        time(&rawtime);
        struct tm* timeinfo = localtime(&rawtime);

        strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);

        char currentTime[84] = "";
        sprintf(currentTime, "%s.%d", buffer, milli);

        return std::string(currentTime);
    }

    INLINE std::time_t timestamp()
    {
        timeval curTime;
        time_of_day(&curTime);
        int milli = curTime.tv_usec / 1000;

        time_t rawtime = time(NULL);
        // tm timeinfo(NULL);
        //		char buffer[80];

        time(&rawtime);
        struct tm* timeinfo = localtime(&rawtime);
        return rawtime;
    }

    std::string timestamp_str(const std::string& aFmt, const bool& aAppendMilliseconds)
    {
        timeval curTime;
        time_of_day(&curTime);
        int milli = curTime.tv_usec / 1000;

        time_t rawtime;
        char buffer[80];

        time(&rawtime);
        struct tm* timeinfo = localtime(&rawtime);

        strftime(&buffer[0], 80, aFmt.c_str(), timeinfo);

        char currentTime[84] = "";
        if (!aAppendMilliseconds)
        {
            sprintf(currentTime, "%s", buffer);
        }
        else
        {
            sprintf(currentTime, "%s.%d", buffer, milli);
        }

        return std::string(currentTime);
    }

#endif

    std::time_t timestamp()
    {
        time_t rawtime = time(NULL);
        tm timeinfo{};
        time(&rawtime);
        localtime_s(&timeinfo, &rawtime);
        return rawtime;
    }

    time_t timestamp_add_seconds(int32_t val, time_t time)
    {
        time_t now{time};
        if (!now)
        {
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

    time_t timestamp_add_minutes(int32_t val, time_t time)
    {
        time_t now{time};
        if (!now)
        {
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

    time_t timestamp_add_hours(int32_t val, time_t time)
    {
        time_t now{time};
        if (!now)
        {
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

    time_t timestamp_add_days(int32_t val, time_t time)
    {
        time_t now{time};
        if (!now)
        {
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

    bool wstr_to_int8(const std::wstring& content, int8_t& val)
    {
        int8_t result{};
        std::string str = wstr_to_str(content);

        if (auto parsed_result = std::from_chars(str.c_str(), str.c_str() + str.length() - 1, result);
            parsed_result.ec == std::errc())
        {
            val = result;
            return true;
        }
        else
        {
            return false;
        }
    }

    bool wstr_to_int16(const std::wstring& content, int16_t& val)
    {
        int16_t result{};
        std::string str = wstr_to_str(content);

        if (auto parsed_result = std::from_chars(str.c_str(), str.c_str() + str.length() - 1, result);
            parsed_result.ec == std::errc())
        {
            val = result;
            return true;
        }
        else
        {
            return false;
        }
    }

    bool wstr_to_int32(const std::wstring& content, int32_t& val)
    {
        int32_t result{};
        std::string str = wstr_to_str(content);

        if (auto parsed_result = std::from_chars(str.c_str(), str.c_str() + str.length() - 1, result);
            parsed_result.ec == std::errc())
        {
            val = result;
            return true;
        }
        else
        {
            return false;
        }
    }

    bool wstr_to_int64(const std::wstring& content, int64_t& val)
    {
        int64_t result{};
        std::string str = wstr_to_str(content);

        if (auto parsed_result = std::from_chars(str.c_str(), str.c_str() + str.length() - 1, result);
            parsed_result.ec == std::errc())
        {
            val = result;
            return true;
        }
        else
        {
            return false;
        }
    }

    bool wstr_to_uint8(const std::wstring& content, uint8_t& val)
    {
        uint8_t result{};
        std::string str = wstr_to_str(content);

        if (auto parsed_result = std::from_chars(str.c_str(), str.c_str() + str.length() - 1, result);
            parsed_result.ec == std::errc())
        {
            val = result;
            return true;
        }
        else
        {
            return false;
        }
    }

    bool wstr_to_uint16(const std::wstring& content, uint16_t& val)
    {
        uint16_t result{};
        std::string str = wstr_to_str(content);

        if (auto parsed_result = std::from_chars(str.c_str(), str.c_str() + str.length() - 1, result);
            parsed_result.ec == std::errc())
        {
            val = result;
            return true;
        }
        else
        {
            return false;
        }
    }

    bool wstr_to_uint32(const std::wstring& content, uint32_t& val)
    {
        uint32_t result{};
        std::string str = wstr_to_str(content);

        if (auto parsed_result = std::from_chars(str.c_str(), str.c_str() + str.length() - 1, result);
            parsed_result.ec == std::errc())
        {
            val = result;
            return true;
        }
        else
        {
            return false;
        }
    }

    bool wstr_to_uint64(const std::wstring& content, uint64_t& val)
    {
        uint64_t result{};
        std::string str = wstr_to_str(content);

        if (auto parsed_result = std::from_chars(str.c_str(), str.c_str() + str.length() - 1, result);
            parsed_result.ec == std::errc())
        {
            val = result;
            return true;
        }
        else
        {
            return false;
        }
    }

    bool str_to_int8(const std::string& content, int8_t& val)
    {
        int8_t result{};
        std::string str{(content)};

        if (auto parsed_result = std::from_chars(str.c_str(), str.c_str() + str.length() - 1, result);
            parsed_result.ec == std::errc())
        {
            val = result;
            return true;
        }
        else
        {
            return false;
        }
    }

    bool str_to_int16(const std::string& content, int16_t& val)
    {
        int16_t result{};
        std::string str{(content)};

        if (auto parsed_result = std::from_chars(str.c_str(), str.c_str() + str.length() - 1, result);
            parsed_result.ec == std::errc())
        {
            val = result;
            return true;
        }
        else
        {
            return false;
        }
    }

    bool str_to_int32(const std::string& content, int32_t& val)
    {
        int32_t result{};
        std::string str{(content)};

        if (auto parsed_result = std::from_chars(str.c_str(), str.c_str() + str.length() - 1, result);
            parsed_result.ec == std::errc())
        {
            val = result;
            return true;
        }
        else
        {
            return false;
        }
    }

    bool str_to_int64(const std::string& content, int64_t& val)
    {
        int64_t result{};
        std::string str{(content)};

        if (auto parsed_result = std::from_chars(str.c_str(), str.c_str() + str.length() - 1, result);
            parsed_result.ec == std::errc())
        {
            val = result;
            return true;
        }
        else
        {
            return false;
        }
    }

    bool str_to_uint8(const std::string& content, uint8_t& val)
    {
        uint8_t result{};
        std::string str{(content)};

        if (auto parsed_result = std::from_chars(str.c_str(), str.c_str() + str.length() - 1, result);
            parsed_result.ec == std::errc())
        {
            val = result;
            return true;
        }
        else
        {
            return false;
        }
    }

    bool str_to_uint16(const std::string& content, uint16_t& val)
    {
        uint16_t result{};
        std::string str{(content)};

        if (auto parsed_result = std::from_chars(str.c_str(), str.c_str() + str.length(), result);
            parsed_result.ec == std::errc())
        {
            val = result;
            return true;
        }
        else
        {
            return false;
        }
    }

    bool str_to_uint32(const std::string& content, uint32_t& val)
    {
        uint32_t result{};
        std::string str{(content)};

        if (auto parsed_result = std::from_chars(str.c_str(), str.c_str() + str.length() - 1, result);
            parsed_result.ec == std::errc())
        {
            val = result;
            return true;
        }
        else
        {
            return false;
        }
    }

    bool str_to_uint64(const std::string& content, uint64_t& val)
    {
        uint64_t result{};
        std::string str{(content)};

        if (auto parsed_result = std::from_chars(str.c_str(), str.c_str() + str.length() - 1, result);
            parsed_result.ec == std::errc())
        {
            val = result;
            return true;
        }
        else
        {
            return false;
        }
    }

    size_t wildcard_pos(const std::string& aText,
                        const std::string& aSubStr,
                        size_t aIndex,
                        const bool& aIgnoreCase,
                        const bool& aWildCard)
    {
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

        do
        {
            char zChar = aText[zSearchIdx];
            char zFindChar = aSubStr[zSubStrIdx];

            if (zChar == zFindChar || (aIgnoreCase && (toupper(zChar) == toupper(zFindChar)))
                || (aWildCard && (zFindChar == '?')))
            {
                if (zSubStrIdx == zLenSub - 1)
                {
                    zResult = zSetIdx;
                    break;
                }

                zSearchIdx++;
                zSubStrIdx++;
            }
            else
            {
                zSetIdx++;
                zSearchIdx = zSetIdx;
                zSubStrIdx = 0;
            }
        } while (zSearchIdx < zLenTxt);

        return zResult;
    }

    size_t replace_substr(std::string& text,
                          const std::string& old_substr,
                          const std::string& new_substr,
                          const bool& ignore_case,
                          const bool& use_wildcard)
    {

        size_t len_old_substr{old_substr.size()};
        if (len_old_substr == 0)
            return 0; // nothing to replace, save us some work

        size_t found_count{0};
        size_t search_idx{0};
        std::array<size_t, 256> found_list{};

        do
        {
            search_idx = wildcard_pos(text, old_substr, search_idx, ignore_case, use_wildcard);
            if (search_idx != std::string::npos)
            {
                if (found_count > 255)
                    throw std::exception("Stoked::Utils::ReplaceText cannot do more than 256 replacements");

                found_list[found_count] = search_idx;

                found_count++;
                search_idx += len_old_substr;
            }
            else
            {
                break;
            }
        } while (true);

        if (found_count > 0)
        {
            size_t len_new_substr = new_substr.size();
            size_t buf_size = text.size() + (found_count * (len_new_substr - len_old_substr));
            std::unique_ptr<char[]> buffer(new char[buf_size + 1]);
            // std::vector<char> buffer(buf_size + 1);

            size_t write_idx{0};
            size_t found_itr{0};
            size_t len_before{0};
            search_idx = 0;

            while (found_itr < found_count)
            {
                len_before = found_list[found_itr] - search_idx;
                memmove(&buffer.get()[write_idx],
                        &text.c_str()[search_idx],
                        len_before * sizeof(char)); // possibly copies 0 chars
                write_idx += len_before;

                if (len_new_substr > 0)
                {
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

    size_t generate_random_byte_str(char* aBuffer, size_t aIndex, size_t aCount)
    {
        // const static char * LOOKUP = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        const static std::string LOOKUP = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        size_t zI = 0;

        while (zI < aCount)
        {
            int zRand = default_random(0, 127);
            char zChar = static_cast<char>(zRand);
            std::string zCharStr(1, zChar);
            if (wildcard_pos(LOOKUP, zCharStr, 0, false, false) >= 0)
            {
                if (aBuffer + zI + aIndex != NULL)
                {
                    *(aBuffer + zI + aIndex) = zChar;
                    zI++;
                }
                else
                {
                    break;
                }
            }
        }

        return zI;
    }

    char* reverse_bytes(std::string val) { return reverse_bytes(val.data(), val.size()); }

    char* reverse_bytes(char* start, size_t size)
    {
        char* s = start;
        char* e = s + size;

        std::reverse(s, e);
        return s;
    }

} // namespace storrent::utils