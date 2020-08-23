#include "storrent/tracker_manager.hpp"
#include "storrent/remote_tracker.hpp"
#include "storrent/uri.hpp"

#include <boost/bind/bind.hpp>
#include <boost/function.hpp>

#include <algorithm>
#include <vector>
#include <string>

namespace storrent
{
    tracker_manager::tracker_manager(torrent_handle htor) : io_ctx{1}, heartbeat{io_ctx}, stopped{false}, htor{htor} {}

    void tracker_manager::start(std::function<int(const std::vector<std::string>&)> get_peers_callback)
    {
        this->on_get_peers = get_peers_callback;

        // check to see if tthe announce is in the announce list, if not we'll add it to the top of the list
        // we add it to the front of the list because we're hoping that since this is 'THE Announce' it will have
        // the best peers and seeds so we can hopefully accumulate enough of them quickly rather than iterating
        // the entire announce list and then having to report to all of the trackers
        auto announces = this->htor->announce_list();
        if (auto found = std::find(announces.begin(), announces.end(), this->htor->announce());
            found == announces.end())
        {
            auto itr = announces.begin();
            announces.insert(itr, this->htor->announce());
        }
        // remove any duplicates before we start processing the list, this may be a waste of time though
        std::sort(announces.begin(), announces.end());
        announces.erase(std::unique(announces.begin(), announces.end()), announces.end());

        // this->io_ctx.run();
        // now lets resolve some tracker hosts and make trackers out of them on successful resolution

        // we're making these locals because hopefully this is only called once per session for a single torrent
        boost::asio::ip::udp::resolver udp_resolver(this->io_ctx);
        boost::asio::ip::tcp::resolver tcp_resolver(this->io_ctx);

        for (auto& announce : announces)
        {
            // if it's a udp url then we'll resolve to udp otherwise...
            auto parsed_uri = uri::parse(announce);

            if (parsed_uri.protocol.starts_with("udp"))
            {
                // I don't think boost::bind is the recommended way of passing the handler
                // callback, it should be a lambda but I think the syntax is crowded and confusing
                // boost::asio::ip::udp::resolver::query q(url);
                udp_resolver.async_resolve(parsed_uri.host,
                                           parsed_uri.port,
                                           boost::bind(&tracker_manager::handle_udp_resolve,
                                                       this,
                                                       boost::asio::placeholders::error,
                                                       boost::asio::placeholders::results));
            }
            else if (parsed_uri.protocol.starts_with("http"))
            {
                tcp_resolver.async_resolve(parsed_uri.host,
                                           parsed_uri.port,
                                           boost::bind(&tracker_manager::handle_tcp_resolve,
                                                       this,
                                                       boost::asio::placeholders::error,
                                                       boost::asio::placeholders::results));
            }
        }

        this->io_ctx_thread = std::thread([this]() { this->io_ctx.run(); });
    }

    void tracker_manager::stop() { this->io_ctx.stop(); }

    // on successful resolution we can now make tracker_manager for them and start fetching some peers,
    void tracker_manager::handle_udp_resolve(const boost::system::error_code& err,
                                             boost::asio::ip::udp::resolver::results_type results)
    {
        udp_remote_tracker tracker(this->io_ctx, this->htor);

        if (!err)
        {
            tracker.start(results, [&](const auto& peers) -> int { return this->handle_peers_list(peers); });
        }
        else
        {
            //// there was an error trying to resolve the udp host so now lets
            //std::cout << "error: " << err.message() << std::endl;
        }
    }

    int tracker_manager::handle_peers_list(const std::vector<std::string>& peers) { return this->on_get_peers(peers); }

    void tracker_manager::handle_tcp_resolve(const boost::system::error_code& err,
                                             boost::asio::ip::tcp::resolver::results_type results)
    {
        /*if (!err)
        {
            std::cout << "success !" << std::endl;
        }
        else
        {
            std::cout << "error: " << err.message() << std::endl;
        }*/
    }
} // namespace storrent