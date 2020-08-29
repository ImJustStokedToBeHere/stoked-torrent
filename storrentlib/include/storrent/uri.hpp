#pragma once
#include <algorithm> // find
#include <string>
#include <string_view>
// #include <utility>

namespace storrent
{
    struct uri
    {
    public:
        std::string_view query_string;
        std::string_view path;
        std::string_view protocol;
        std::string_view host;
        std::string_view port;
        std::string url;

        static uri parse(const std::string& uri_str)
        {
            uri result;

            // typedef std::string::const_iterator iterator_t;
            result.url = uri_str;

            if (uri_str.length() == 0)
                return result;

            auto uriEnd = uri_str.end();

            // get query start
            auto queryStart = std::find(uri_str.begin(), uriEnd, '?');

            // protocol
            auto protocolStart = uri_str.begin();
            auto protocolEnd = std::find(protocolStart, uriEnd, ':'); //"://");

            if (protocolEnd != uriEnd)
            {
                std::string prot = &*(protocolEnd);
                if ((prot.length() > 3) && (prot.substr(0, 3) == "://"))
                {
                    // result.protocol = std::string_view(protocolStart, protocolEnd);
                    // result.protocol = std::string_view(protocolStart, std::difference(protocolEnd, protocolStart));
                    
                    auto c = protocolStart;
                    result.protocol = std::string_view{&*protocolStart, size_t(std::distance(protocolEnd, protocolStart))};
                    protocolEnd += 3; //      ://
                }
                else
                    protocolEnd = uri_str.begin(); // no protocol
            }
            else
                protocolEnd = uri_str.begin(); // no protocol

            // host
            auto hostStart = protocolEnd;
            auto pathStart = std::find(hostStart, uriEnd, '/'); // get pathStart

            auto hostEnd = std::find(protocolEnd,
                                     (pathStart != uriEnd) ? pathStart : queryStart,
                                     ':'); // check for port

            result.host = std::string_view(&*hostStart, size_t(std::distance(hostEnd, hostStart)));

            // port
            if ((hostEnd != uriEnd) && ((&*(hostEnd))[0] == ':')) // we have a port
            {
                hostEnd++;
                auto portEnd = (pathStart != uriEnd) ? pathStart : queryStart;
                result.port = std::string_view(&*hostEnd, size_t(std::distance(portEnd, hostEnd)));
            }

            // path
            if (pathStart != uriEnd)
                result.path = std::string_view(&*pathStart, size_t(std::distance(queryStart, pathStart)));

            // query
            if (queryStart != uriEnd)
                result.query_string = std::string_view(&*queryStart, size_t(std::distance(uri_str.end(), queryStart)));

            return result;

        } // parse
    };    // uri

    // constexpr auto s = sizeof(uri);

} // namespace storrent0