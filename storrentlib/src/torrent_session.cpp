#include "storrent/torrent_session.hpp"

namespace storrent
{
    std::shared_ptr<torrent_session> torrent_session::make(const std::string& torrent_filename,
                                                                  const std::string& source_file_directory_name)
    {
        if (!utils::file_exists(torrent_filename))
            return nullptr;

        auto htor = torrent::load_torrent_file(torrent_filename);

        return std::make_shared<torrent_session>(htor, source_file_directory_name);
    }

    void torrent_session::start_download()
    {
        // here is where we need to figure out what pieces we have, if the file is complete then we have all of them
        // trackers_ptr->start_io();
        if (trackers_ptr->is_ready().get())
        {
            auto hashes = get_hashes();
            auto scrape_results = trackers_ptr->get_scrape_results(hashes);
        }
        
    }
} // namespace storrent