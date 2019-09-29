#pragma once
#include <string>
#include <regex>
#include <optional>

namespace stoked::btp {

    namespace {
        inline static const std::regex expr("(http|https|udp)://([^/ :]+):?([^/ ]*)(/?[^ #?]*)\\x3f?([^ #]*)#?([^ ]*)", std::regex::optimize);
    }

    struct uri {
    public:
        static std::optional<uri> parse(const std::string& url_str) {
            // expr.
            std::cmatch what;
            if (std::regex_match(url_str.c_str(), what, expr, std::regex_constants::match_default)) {
                return uri{what, url_str};
            } else {
                return std::nullopt;
            }
        }

        std::string protocol() const {
            return {_match[1].first, _match[1].second};
        }

        std::string domain() const {
            return {_match[2].first, _match[2].second};
        }

        std::string port() const {
            return {_match[3].first, _match[3].second};
        }

        std::string path() const {
            return {_match[4].first, _match[4].second};
        }

        std::string query() const {
            return {_match[5].first, _match[5].second};
        }

        std::string fragment() const {
            return {_match[6].first, _match[6].second};
        }

        uri(const uri&) = delete;
        uri(uri&&) = default;

        uri& operator=(const uri&) = delete;
        uri& operator=(uri&&) = default;

    private:
        uri(std::cmatch match, std::string str_val)
            : _str_val{std::move(str_val)},
            _match{std::move(match)}
        { }

        std::string _str_val;
        std::cmatch _match;
    };

    const int f = sizeof(uri);
}