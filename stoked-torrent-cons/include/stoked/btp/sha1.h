#pragma once
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace stoked::btp::utils {
    class sha1 {
    public:
        sha1();
        void update(const std::string& s);
        void update(std::istream& is);
        std::string final();
        void reset_digest();

        const uint32_t* const raw_digest() const {
            return digest;
        }

        static std::string from_file(const std::string& filename);
        static std::string from_file(const std::wstring& filename);
        static std::string from_str(const std::string& msg_str);
        static std::string from_str(const std::wstring& msg_str);
        void set_final(std::string final);
    private:
        std::string _result_str{};
        uint32_t digest[5];
        std::string buffer;
        uint64_t transforms;
    };
}
