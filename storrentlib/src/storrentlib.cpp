// storrentlib.cpp : Defines the functions for the static library.
#include "storrent/storrentlib.hpp"
#include "storrent/engine.hpp"
#include "storrent/torrent_session.hpp"
#include "storrent/utils.hpp"

namespace storrent
{
    namespace
    {
        engine main_engine;
    }

    info_hash start_session(const std::string& filename, const std::string& dir_name)
    {
        // make sure our file exists
        if (!utils::file_exists(filename))
            return torrent_session::INVALID_ID;
        // lets check if our engine is already running
        if (!main_engine.is_running())
        {
            main_engine.start();
        }
        



        // TODOTODO: check to see if there is a resume file for this torrent
        bool is_resume_file = false;
        auto sesh = is_resume_file ? torrent_session::resume(filename) : torrent_session::make(filename, dir_name);

        return sesh->get_id();
    }

    info_hash get_download_progress(int download_id, download_progress& progress)
    {
        return torrent_session::INVALID_ID;
    }

} // namespace storrent
