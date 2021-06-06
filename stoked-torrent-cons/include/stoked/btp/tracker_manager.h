#pragma once
#include <boost/asio.hpp>
#include <set>

namespace stoked::btp
{
    using boost::asio::io_service;

    struct tracker_manager
    {
        explicit tracker_manager(io_service& ios) noexcept
            : _ios{ios}
        { }

        size_t add_endpoint(const std::string& tracker_url)
        {

        }

    private:
        std::set<boost::asio::ip::udp::endpoint> m_udp_trackers;
        std::set<boost::asio::ip::tcp::endpoint> m_tcp_trackers;
        io_service& _ios{nullptr};
    };
}
