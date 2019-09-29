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

        void raw(uint32_t* raw) {
            raw[0] = digest[0];
            raw[1] = digest[1];
            raw[2] = digest[2];
            raw[3] = digest[3];
            raw[4] = digest[4];
        }

        static std::string from_file(const std::string& filename);
        static std::string from_file(const std::wstring& filename);
        static std::string from_str(const std::string& msg_str);
        static std::string from_str(const std::wstring& msg_str);

    private:
        uint32_t digest[5];
        std::string buffer;
        uint64_t transforms;
    };
}
