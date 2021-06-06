#pragma once
#include "storrent/torrent.hpp"
#include "storrent/piece_block.hpp"
#include <queue>

namespace storrent
{
    struct piece_queue
    {
        piece_queue(torrent_handle htor) 
            : m_htor{htor}, m_choked{true}
        { }

        piece_queue(const piece_queue&) = default;
        piece_queue(piece_queue&&) = default;

        ~piece_queue() {}

        piece_queue& operator=(const piece_queue&) = default;
        piece_queue& operator=(piece_queue&&) = default;

        piece_block deque() 
        { 
            auto result = m_q.front();
            m_q.pop();
            return result;
        }

        piece_block peek() const { return m_q.front(); }

        size_t len() const { return m_q.size(); }

    private:
        torrent_handle m_htor{nullptr};
        std::queue<piece_block> m_q;
        bool m_choked;
    };
}