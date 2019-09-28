#pragma once

#include <string>
#include <crtdbg.h>
#include <Windows.h>
#include <Psapi.h>

namespace stoked::utils {
    struct drive_list {
        int count{0};
        std::string volumes[26];
    };

    class performance_timer {
    private:
        bool _running;
        long long _start;
        long long _stop;
        const long long _frequency;
        performance_timer(const performance_timer&) = delete;
        performance_timer& operator=(const performance_timer&) = delete;
        static long long counter(void);
        static long long frequency(void);
        double elapsed_ms(long long aStart, long long aStop) const;

    public:
        performance_timer(void);
        ~performance_timer(void);
        void start(void);
        void stop(void);
        void reset(void);
        double elapsed_ms(void) const;
    };

    //struct periodic_counter {
    //	performance_timer LastUpdate;
    //	unsigned long long LastValue;
    //	ULARGE_INTEGER LastCPU;
    //	ULARGE_INTEGER LastUserCPU;
    //	ULARGE_INTEGER LastSysCPU;
    //	long double LastCPUPercent;
    //	long double LastMemPercent;
    //};

    // inclusive
    enum class file_version_info_value {
        comments = 0,
        company_name,
        file_description,
        file_version,
        internal_name,
        legal_copyright,
        legal_trademarks,
        original_filename,
        product_name,
        product_version,
        private_build,
        special_build
    };

    class module_info {
    private:
        HMODULE _module;
        bool _refresh[12];
        bool _init;
        std::string _filename;
        std::string _file_dir;
        std::string _file_version_info_val[12];


        const std::string& get_val(file_version_info_value aValue);
        void set_val(file_version_info_value aValue);

        module_info(const module_info&) = delete;
        module_info& operator=(const module_info&) = delete;
    public:
        module_info(void);
        module_info(HMODULE aModule); // this could be any DLL this process has loaded
        ~module_info(void);

        const std::string& module_filename(void) const;
        const std::string& comments(void);
        const std::string& company_name(void);
        const std::string& file_description(void);
        const std::string& file_version(void);
        const std::string& internal_name(void);
        const std::string& legal_copyright(void);
        const std::string& legal_trademarks(void);
        const std::string& original_filename(void);
        const std::string& product_name(void);
        const std::string& product_version(void);
        const std::string& private_build(void);
        const std::string& special_build(void);

        void set_refresh(size_t aRefreshIdx);
        void refresh(void);

        static std::string get_file_version_info_value(const std::string& aFileName, file_version_info_value aValue);
        static std::string get_file_version_info_value(file_version_info_value val);
        static std::string get_file_name_from_module(HMODULE aModuleHandle = NULL);
    };

    drive_list get_storage_volumes(void);
    unsigned long long get_total_virtual_memory(void);
    unsigned long long get_virtual_mem_in_use(void);
    unsigned long long get_virtual_mem_used_by_process(void);
    unsigned long long get_total_physical_mem(void);
    unsigned long long get_physical_mem_in_use(void);
    unsigned long long get_physical_mem_used_by_process(void);
    double get_total_cpu_usage(void);
    double get_cpu_used_by_process(void);
    unsigned int get_processor_count(void);

    // int GenerateRandomNumberInRange(const int &aStartRange, const int &aEndRange);


    typedef GUID SGUID;
    bool generate_guid(SGUID* aGuid);
}

