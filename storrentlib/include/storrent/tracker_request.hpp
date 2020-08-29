#pragma once
#include <cstdint>
#include <string>
#include <limits>

#define BYTES_ADDR(x) (static_cast<const char*>(static_cast<const void*>(&x)))
#define BYTES_ADDR_MUT(x) (static_cast<char*>(static_cast<void*>(&x)))
#define BYTES_ADDR_OFFSET(x, n) (static_cast<const char*>(static_cast<const void*>(&x)) + n)
#define BYTES_ADDR_OFFSET_MUT(x, n) (static_cast<char*>(static_cast<void*>(&x)) + n)

namespace storrent
{
    struct tracker_request
    {
        virtual size_t field_count() const = 0;
        virtual size_t field_offset(size_t field_idx) const = 0;
        virtual std::pair<const char*, size_t> field_data(size_t field_idx) const = 0;
        virtual std::string field_name(size_t field_idx) const = 0;
    };

    struct scrape_request : public tracker_request
    {
    };

    struct announce_request : public tracker_request
    {
    };

    struct connect_request : public tracker_request
    {
        virtual size_t field_count() const noexcept override 
        { 
            return 3;
        }

        virtual size_t field_offset(size_t field_idx) const override 
        {
            switch (field_idx)
            {
                case 0:
                    return 0;
                case 1:
                    return 8;
                case 2:
                    return 12;
                default:
                    return std::numeric_limits<size_t>::max();
            }
        }

        virtual std::pair<const char*, size_t> field_data(size_t field_idx) const override
        {
            switch (field_idx)
            {
                case 0:
                    return std::make_pair(BYTES_ADDR(this->protocol_id), 8);
                case 1:
                    return std::make_pair(BYTES_ADDR(this->action), 4);
                case 2:
                    return std::make_pair(BYTES_ADDR(this->tx_id), 4);
                default:
                    return std::make_pair(nullptr, 0);
            }
        }

        virtual std::string field_name(size_t field_idx) const override
        {
            switch (field_idx)
            {
                case 0:
                    return "protocol id";
                case 1:
                    return "action";
                case 2:
                    return "transaction_id";
                default:
                    return "";
            }
        }

        std::int64_t protocol_id{-1};
        std::int32_t action{-1};
        std::int32_t tx_id{-1};
    };

    enum class announce_event
    {
        none,
        completed,
        started,
        stopped
    };
}