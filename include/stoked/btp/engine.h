#pragma once
#include <cstdint>
#include <vector>
#include <memory>
#include <set>

namespace stoked::btp {
    struct session;

    struct bt_engine_settings {

    };

    struct bt_engine  {
    public:
        static bt_engine_settings default_engine_settings() {
            return {};
        }

        enum class status {
            engine_created,
            engine_started,
            engine_running,
            engine_stopped,
            engine_errored
        };

        bt_engine()
            : bt_engine(default_engine_settings()) {

        }

        bt_engine(bt_engine_settings settings)
            : _settings{std::move(settings)} {

        }

        bool add_session(std::shared_ptr<session> sess) {
            if (_sessions.find(sess) == _sessions.end()) {
                _sessions.emplace(sess);
                return true;
            } else {
                return false;
            }
        }

    private:
        status _status{status::engine_created};
        bt_engine_settings _settings;
        std::set<std::shared_ptr<session>> _sessions;
    };
}

