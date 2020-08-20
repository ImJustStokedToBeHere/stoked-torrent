#include "storrent/sys_utils.hpp"
#if defined(_MSC_VER) || defined(WIN_VER)
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #include <Psapi.h>
    #include <Pdh.h>
    #include <Rpc.h>
#else
    /*#include <arpa/inet.h>
    #include <netdb.h>
    #include <sys/time.h>

    #ifndef OutputDebugString
        #include <iostream>
        #define OutputDebugString(str) std::cout << str << std::endl;
    #endif*/
#endif
#include <sstream>
#include <random>


#define INTERVAL 0.0001157407407
#define SHORT_INTERVAL 1 / 86400
#define INFO_SELECTOR_A TEXT("StringFileInfo\\040904E4\\")
#define INFO_SELECTOR_W TEXT("StringFileInfo\\040904B0\\")

namespace storrent::sys
{
    std::string module_info::get_file_name_from_module(HMODULE aModuleHandle)
    {
        // char* filename = NULL;
        std::unique_ptr<char[]> filename;
        DWORD copied = 0;
        size_t filename_len = 0;
        do
        {
            filename.reset((LPSTR)realloc(filename.get(), filename_len + MAX_PATH));

            copied = GetModuleFileNameEx(GetCurrentProcess(),
                                         aModuleHandle,
                                         static_cast<LPSTR>(filename.get()),
                                         (DWORD)strlen(filename.get()));
            if (!copied)
            {
                return "";
            }

            filename_len = strlen(filename.get());
        } while (filename_len < copied);

        if (copied < strlen(filename.get()))
        {
            filename.reset((LPSTR)realloc(static_cast<void*>(filename.get()), copied));
            filename.get()[copied] = (char)'#0';
        }

        std::string result(filename.get(), copied);
        // free(filename);
        return result;
    }

    module_info::module_info(void) : module_info(NULL) {}

    module_info::module_info(HMODULE aModule)
    {
        _module = aModule;
        _refresh[0] = true;
        _refresh[1] = true;
        _refresh[2] = true;
        _refresh[3] = true;
        _refresh[4] = true;
        _refresh[5] = true;
        _refresh[6] = true;
        _refresh[7] = true;
        _refresh[8] = true;
        _refresh[9] = true;
        _refresh[10] = true;
        _refresh[11] = true;

        _filename = get_file_name_from_module(_module);
        _init = !_filename.empty();
    }

    module_info::~module_info(void) {}

    void module_info::set_val(file_version_info_value aValue)
    {
        _file_version_info_val[static_cast<size_t>(aValue)] = module_info::get_file_version_info_value(_filename,
                                                                                                       aValue);
    }

    const std::string& module_info::get_val(file_version_info_value aValue)
    {
        size_t int_val = static_cast<size_t>(aValue);
        if (_refresh[int_val])
        {
            _refresh[int_val] = false;
            set_val(aValue);
        }

        return _file_version_info_val[int_val];
    }

    const std::string& module_info::module_filename(void) const { return _filename; }

    void module_info::set_refresh(size_t aRefreshIdx) { _refresh[aRefreshIdx] = true; }

    void module_info::refresh(void)
    {
        for (size_t i = 0; i < sizeof(_refresh); i++)
        {
            set_refresh(i);
        }
    }

    const std::string& module_info::comments(void) { return this->get_val(file_version_info_value::comments); }

    const std::string& module_info::company_name(void) { return this->get_val(file_version_info_value::company_name); }

    const std::string& module_info::file_description(void)
    {
        return this->get_val(file_version_info_value::file_description);
    }

    const std::string& module_info::file_version(void) { return this->get_val(file_version_info_value::file_version); }

    const std::string& module_info::internal_name(void)
    {
        return this->get_val(file_version_info_value::internal_name);
    }

    const std::string& module_info::legal_copyright(void)
    {
        return this->get_val(file_version_info_value::legal_copyright);
    }

    const std::string& module_info::legal_trademarks(void)
    {
        return this->get_val(file_version_info_value::legal_trademarks);
    }

    const std::string& module_info::original_filename(void)
    {
        return this->get_val(file_version_info_value::original_filename);
    }

    const std::string& module_info::product_name(void) { return this->get_val(file_version_info_value::product_name); }

    const std::string& module_info::product_version(void)
    {
        return this->get_val(file_version_info_value::product_version);
    }

    const std::string& module_info::private_build(void)
    {
        return this->get_val(file_version_info_value::private_build);
    }

    const std::string& module_info::special_build(void)
    {
        return this->get_val(file_version_info_value::special_build);
    }

    std::string module_info::get_file_version_info_value(file_version_info_value val)
    {
        return get_file_version_info_value(get_file_name_from_module(), val);
    }

    std::string module_info::get_file_version_info_value(const std::string& aFileName, file_version_info_value aValue)
    {
        DWORD hnd = NULL;
        DWORD infosize = GetFileVersionInfoSize(aFileName.c_str(), &hnd);

        if (infosize)
        {
            LPVOID data = malloc(infosize);
            if (GetFileVersionInfo(aFileName.c_str(), hnd, infosize, data))
            {
                std::stringstream zSS;
                PUINT data_len = NULL;
                LPVOID actual_data = NULL;

                std::string info_selector_a = INFO_SELECTOR_A;
                std::string info_selector_w = INFO_SELECTOR_W;
                std::string zValueName;
                switch (aValue)
                {
                    case file_version_info_value::comments:
                    {
                        zValueName = "Comments";
                        break;
                    }
                    case file_version_info_value::company_name:
                    {
                        zValueName = "CompanyName";
                        break;
                    }
                    case file_version_info_value::file_description:
                    {
                        zValueName = "FileDescription";
                        break;
                    }
                    case file_version_info_value::file_version:
                    {
                        zValueName = "FileVersion";
                        break;
                    }
                    case file_version_info_value::internal_name:
                    {
                        zValueName = "InternalName";
                        break;
                    }
                    case file_version_info_value::legal_copyright:
                    {
                        zValueName = "LegalCopyright";
                        break;
                    }
                    case file_version_info_value::legal_trademarks:
                    {
                        zValueName = "LegalTrademarks";
                        break;
                    }
                    case file_version_info_value::original_filename:
                    {
                        zValueName = "OriginalFilename";
                        break;
                    }
                    case file_version_info_value::product_name:
                    {
                        zValueName = "ProductName";
                        break;
                    }
                    case file_version_info_value::product_version:
                    {
                        zValueName = "ProductVersion";
                        break;
                    }
                    case file_version_info_value::private_build:
                    {
                        zValueName = "PrivateBuild";
                        break;
                    }
                    case file_version_info_value::special_build:
                    {
                        zValueName = "SpecialBuild";
                        break;
                    }
                }

                info_selector_a += zValueName;
                info_selector_w += zValueName;

                if (VerQueryValue(data, info_selector_a.c_str(), &actual_data, data_len))
                {
                    zSS << (char*)actual_data;
                }
                else if (VerQueryValue(data, info_selector_w.c_str(), &actual_data, data_len))
                {
                    zSS << (char*)actual_data;
                }

                return zSS.str();
            }
        }

        return "";
    }

#pragma region PERFORMANCE TIMER

#ifndef _DEBUG
    inline
#endif
        performance_timer::performance_timer(void)
        : _frequency{frequency()}
    {
        _running = false;
        _start = 0;
        _stop = 0;
    }

#ifndef _DEBUG
    inline
#endif
        performance_timer::~performance_timer(void)
    {
    }

#ifndef _DEBUG
    inline
#endif
        void
        performance_timer::reset(void)
    {
        _stop = _start = 0;
        _running = false;
    }

#ifndef _DEBUG
    inline
#endif
        void
        performance_timer::start(void)
    {
        _running = true;
        _stop = 0;
        _start = counter();
    }

#ifndef _DEBUG
    inline
#endif
        void
        performance_timer::stop(void)
    {
        _stop = counter();
        _running = false;
    }

#ifndef _DEBUG
    inline
#endif
        double
        performance_timer::elapsed_ms(void) const
    {
        if (_running)
        {
            const long long zCurrent = counter();
            return elapsed_ms(_start, zCurrent);
        }

        return elapsed_ms(_start, _stop);
    }

#ifndef _DEBUG
    inline
#endif
        long long
        performance_timer::counter(void)
    {
        LARGE_INTEGER zLI;
        QueryPerformanceCounter(&zLI);
        return zLI.QuadPart;
    }

#ifndef _DEBUG
    inline
#endif
        double
        performance_timer::elapsed_ms(long long aStart, long long aStop) const
    {
        _ASSERTE(aStart >= 0);
        _ASSERTE(aStop >= 0);
        _ASSERTE(aStart <= aStop);
        return ((aStop - aStart) * 1000.0) / _frequency;
    }

#ifndef _DEBUG
    inline
#endif
        long long
        performance_timer::frequency(void)
    {
        LARGE_INTEGER zLI;
        QueryPerformanceFrequency(&zLI);
        return zLI.QuadPart;
    }
#pragma endregion

    drive_list get_storage_volumes(void)
    {
        drive_list storage_volumes;
        DWORD size = MAX_PATH;
        char drive_str[MAX_PATH] = {0};
        DWORD result = GetLogicalDriveStrings(size, drive_str);
        if (result > 0 && result <= MAX_PATH)
        {
            int idx = 0;
            char* drive = drive_str;
            while (*drive && idx < 26)
            {
                storage_volumes.volumes[idx] = std::string(drive);
                drive += strlen(drive) + 1;
                idx++;
            }

            storage_volumes.count = idx;
        }

        return storage_volumes;
    }

    unsigned long long get_total_virtual_memory(void)
    {
        MEMORYSTATUSEX mem_info;
        mem_info.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&mem_info);
        return mem_info.ullTotalPageFile;
    }

    unsigned long long get_virtual_mem_in_use(void)
    {
        MEMORYSTATUSEX mem_info;
        mem_info.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&mem_info);
        return mem_info.ullTotalPageFile - mem_info.ullAvailPageFile;
    }

    unsigned long long get_virtual_mem_used_by_process(void)
    {
        PROCESS_MEMORY_COUNTERS_EX zProcMemCntrs;
        GetProcessMemoryInfo(GetCurrentProcess(), (PPROCESS_MEMORY_COUNTERS)&zProcMemCntrs, sizeof(zProcMemCntrs));
        return zProcMemCntrs.PrivateUsage;
    }

    unsigned long long get_total_physical_mem(void)
    {
        MEMORYSTATUSEX mem_info;
        mem_info.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&mem_info);
        return mem_info.ullTotalPhys;
    }

    unsigned long long get_physical_mem_in_use(void)
    {
        MEMORYSTATUSEX mem_info;
        mem_info.dwLength = sizeof(MEMORYSTATUSEX);
        GlobalMemoryStatusEx(&mem_info);
        return mem_info.ullTotalPhys - mem_info.ullAvailPhys;
    }

    unsigned long long get_physical_mem_used_by_process(void)
    {
        PROCESS_MEMORY_COUNTERS_EX zProcMemCntrs;
        GetProcessMemoryInfo(GetCurrentProcess(), (PPROCESS_MEMORY_COUNTERS)&zProcMemCntrs, sizeof(zProcMemCntrs));
        return zProcMemCntrs.WorkingSetSize;
    }

    double get_total_cpu_usage(void)
    {
        static PDH_HQUERY cpu_query;
        static PDH_HCOUNTER cpu_total;

        PdhOpenQuery(NULL, NULL, &cpu_query);
        PdhAddEnglishCounter(cpu_query, "\\Processor(_Total)\\% Processor Time", NULL, &cpu_total);
        PdhCollectQueryData(cpu_query);

        PDH_FMT_COUNTERVALUE cnt_val;

        PdhCollectQueryData(cpu_query);
        PdhGetFormattedCounterValue(cpu_total, PDH_FMT_DOUBLE, NULL, &cnt_val);
        return cnt_val.doubleValue;
    }

    bool initCPUByProc = false;

    static ULARGE_INTEGER zLastCPU, zLastSysCPU, zLastUserCPU;
    static int zNumProcessors;
    static HANDLE zThisProcess = GetCurrentProcess();

    void InitCPUByProc(void)
    {
        SYSTEM_INFO zSysInfo;
        FILETIME zTime, zSys, zUser;

        GetSystemInfo(&zSysInfo);
        zNumProcessors = get_processor_count();

        GetSystemTimeAsFileTime(&zTime);
        memcpy(&zLastCPU, &zTime, sizeof(FILETIME));

        // zThisProcess ;
        GetProcessTimes(zThisProcess, &zTime, &zTime, &zSys, &zUser);
        memcpy(&zLastSysCPU, &zSys, sizeof(FILETIME));
        memcpy(&zLastUserCPU, &zUser, sizeof(FILETIME));

        initCPUByProc = true;
    }

    double get_cpu_used_by_process(void)
    {
        if (!initCPUByProc)
            InitCPUByProc();

        FILETIME ftime, fsys, fuser;
        ULARGE_INTEGER now, sys, user;
        double percent;

        GetSystemTimeAsFileTime(&ftime);
        memcpy(&now, &ftime, sizeof(FILETIME));

        GetProcessTimes(zThisProcess, &ftime, &ftime, &fsys, &fuser);
        memcpy(&sys, &fsys, sizeof(FILETIME));
        memcpy(&user, &fuser, sizeof(FILETIME));
        percent = (double)(sys.QuadPart - zLastSysCPU.QuadPart) + (user.QuadPart - zLastUserCPU.QuadPart);
        percent /= (now.QuadPart - zLastCPU.QuadPart);
        percent /= zNumProcessors;
        zLastCPU = now;
        zLastUserCPU = user;
        zLastSysCPU = sys;

        return percent * 100;
    }

    unsigned int get_processor_count(void)
    {
        SYSTEM_INFO zSysInfo;
        GetSystemInfo(&zSysInfo);
        return zSysInfo.dwNumberOfProcessors;
    }

    bool generate_guid(SGUID* aGUID)
    {
        RPC_STATUS zRet = UuidCreate(aGUID);
        if (zRet == RPC_S_OK || zRet == RPC_S_UUID_LOCAL_ONLY)
            return true;
        else
            return false;
    }
}