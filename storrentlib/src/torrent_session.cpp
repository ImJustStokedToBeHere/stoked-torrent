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
        //this->trackers_ptr->start_socket(std::bind(&torrent_session::handle_peers, this, std::placeholders::_1));




    }
} // namespace storrent