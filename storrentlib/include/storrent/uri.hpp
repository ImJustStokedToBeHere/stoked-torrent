#pragma once
#include <algorithm> // find
#include <string>

namespace storrent
{

    struct uri
    {
    public:
        std::string query_string, path, protocol, host, port;

        static uri parse(const std::string& uri_str)
        {
            uri result;

            // typedef std::string::const_iterator iterator_t;

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
                    result.protocol = std::string(protocolStart, protocolEnd);
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

            result.host = std::string(hostStart, hostEnd);

            // port
            if ((hostEnd != uriEnd) && ((&*(hostEnd))[0] == ':')) // we have a port
            {
                hostEnd++;
                auto portEnd = (pathStart != uriEnd) ? pathStart : queryStart;
                result.port = std::string(hostEnd, portEnd);
            }

            // path
            if (pathStart != uriEnd)
                result.path = std::string(pathStart, queryStart);

            // query
            if (queryStart != uriEnd)
                result.query_string = std::string(queryStart, uri_str.end());

            return result;

        } // parse
    };    // uri

} // namespace storrent0