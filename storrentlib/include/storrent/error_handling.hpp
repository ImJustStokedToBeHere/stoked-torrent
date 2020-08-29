#pragma once
#include "storrent/utils.hpp"
#include <stdexcept>

namespace storrent
{
    class not_implemented : public std::logic_error
    {
    public:
        not_implemented(const char* msg, std::string_view file, int line)
            : std::logic_error(msg), file{file}, line{line}
        {
        }

        virtual char const* what() const
        {
            auto s = utils::make_str("function not implemented at file: ",
                                     file,
                                     " line: ",
                                     line,
                                     ", except message: ",
                                     std::logic_error::what());

            return s.c_str();
        }

    private:
        std::string_view file;
        int line;
    };

}

