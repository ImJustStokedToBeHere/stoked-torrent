#pragma once
#include "storrent/torrent_session.hpp"
#include "storrent/info_hash.hpp"
#include <shared_mutex>

namespace storrent
{
    struct torrent_session;

    struct engine_options
    {
        constexpr engine_options(int listen_port_opt,
                                 int upload_speed_opt,
                                 int dl_speed_opt,
                                 int max_connections_opt,
                                 int upload_slots_opt,
                                 int max_disk_rd,
                                 int max_disk_wrt)
            : listen_port{listen_port_opt},
              upload_speed{upload_speed_opt},
              download_speed{dl_speed_opt},
              max_connections{max_connections_opt},
              upload_slots{upload_slots_opt},
              max_disk_read{max_disk_rd},
              max_disk_write{max_disk_wrt}
        {
        }

        constexpr engine_options(const engine_options&) = default;
        constexpr engine_options(engine_options&&) = default;

        constexpr engine_options& operator=(const engine_options&) noexcept = default;
        constexpr engine_options& operator=(engine_options&&) noexcept = default;

        constexpr int listen_port_num() const { return this->listen_port; }
        constexpr int max_upload_speed() const { return this->upload_speed; }
        constexpr int max_download_speed() const { return this->download_speed; }
        constexpr int max_conncurrent_connections() const { return this->max_connections; }
        constexpr int max_upload_slots() const { return this->upload_slots; }
        constexpr int max_disk_read_rate() const { return this->max_disk_read; }
        constexpr int max_disk_write_rate() const { return this->max_disk_write; }

    private:
        int listen_port;
        int upload_speed;
        int download_speed;
        int max_connections;
        int upload_slots;
        int max_disk_read;
        int max_disk_write;
    };

    static constexpr engine_options DEFAULT_OPTIONS = {6666, 0, 0, 50, 5, 0, 0};

    struct engine
    {
        engine(engine_options opt = DEFAULT_OPTIONS) : stopped{false} {}

        void register_session(const info_hash& hash, std::shared_ptr<torrent_session> sesh) 
        { 
            std::unique_lock lock(this->mutx);
            this->sesh_map.insert(std::make_pair(hash, sesh));
        }

        std::shared_ptr<torrent_session> get_session(const info_hash& hash) 
        { 
            std::shared_lock lock(this->mutx);
            if (auto found = this->sesh_map.find(hash); found != this->sesh_map.end())
            {
                return found->second;
            }
            
            return nullptr;
        }

        void start() 
        { 
            this->stopped = false; 

        }

        bool is_running() const { return !this->stopped; }
    private:
        std::unordered_map<info_hash, std::shared_ptr<torrent_session>> sesh_map;
        std::shared_mutex mutx;
        bool stopped;
    };

    
} // namespace storrent
