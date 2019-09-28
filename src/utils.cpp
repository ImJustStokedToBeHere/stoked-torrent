#include "utils.h"

namespace stoked::utils {

    std::chrono::milliseconds chrono_timestamp() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch());
    }
#if defined(_MSC_VER) || defined(WIN_VER)
    typedef struct timeval {
        long tv_sec;
        long tv_usec;
    } timeval;
#endif

#if defined(_MSC_VER) || defined(WIN_VER)
    int64_t time_of_day(timeval* tp) {
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

    std::wstring timestamp_str_w() {
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


    std::string timestamp_str_a() {
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

    std::time_t timestamp() {
        time_t rawtime = time(NULL);
        tm timeinfo{};
        time(&rawtime);
        localtime_s(&timeinfo, &rawtime);
        return rawtime;
    }

    std::string timestamp_str_fmt_a(const std::string& aFmt,
                                    const bool& aAppendMilliseconds) {
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
        if (!aAppendMilliseconds) {
            sprintf_s(currentTime, "%s", buffer);
        } else {
            sprintf_s(currentTime, "%s.%d", buffer, milli);
        }

        return std::string(currentTime);
    }

    std::string timestamp_str_fmt_a(const std::string& aFmt, time_t val) {

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

    std::wstring timestamp_str_fmt_w(const std::wstring& aFmt,
                                     const bool& aAppendMilliseconds) {
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
        if (!aAppendMilliseconds) {
            swprintf_s(currentTime, L"%s", buffer);
        } else {
            swprintf_s(currentTime, L"%s.%d", buffer, milli);
        }

        return std::wstring(currentTime);
    }

    std::wstring timestamp_str_fmt_w(const std::wstring& aFmt, time_t val) {

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

    int64_t time_of_day(timeval* tp) {
        gettimeofday(tp, nullptr);  // get current time
        long long milliseconds =
            tp->tv_sec * 1000LL + tp->tv_usec / 1000;  // calculate milliseconds
        // printf("milliseconds: %lld\n", milliseconds);
        return milliseconds;
    }

    std::string timestamp_str() {
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

    INLINE std::time_t timestamp() {
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

    std::string timestamp_str(const std::string& aFmt,
                              const bool& aAppendMilliseconds) {
        timeval curTime;
        time_of_day(&curTime);
        int milli = curTime.tv_usec / 1000;

        time_t rawtime;
        char buffer[80];

        time(&rawtime);
        struct tm* timeinfo = localtime(&rawtime);

        strftime(&buffer[0], 80, aFmt.c_str(), timeinfo);

        char currentTime[84] = "";
        if (!aAppendMilliseconds) {
            sprintf(currentTime, "%s", buffer);
        } else {
            sprintf(currentTime, "%s.%d", buffer, milli);
        }

        return std::string(currentTime);
    }

#endif
}