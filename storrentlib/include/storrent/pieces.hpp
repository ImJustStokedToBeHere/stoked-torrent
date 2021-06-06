#pragma once
#include "storrent/piece_block.hpp"
#include "storrent/torrent.hpp"
#include <vector>

namespace storrent
{
    struct pieces
    {
        pieces(torrent_handle htor)
        {
            m_reqd = make_piece_array(htor);
            m_recvd = make_piece_array(htor);
        }

        pieces(const pieces&) = default;
        pieces(pieces&&) = default;

        ~pieces() {}

        pieces& operator=(const pieces&) = default;
        pieces& operator=(pieces&&) = default;

        void add_requested(piece_block pc_blk)
        {
            const size_t blk_idx = pc_blk.begin / torrent::BLOCK_LEN;
            m_reqd[pc_blk.piece_idx][blk_idx] = true;
        }

        void add_received(piece_block pc_blk)
        {
            const size_t blk_idx = pc_blk.begin / torrent::BLOCK_LEN;
            m_recvd[pc_blk.piece_idx][blk_idx] = true;
        }

        bool needed(piece_block blk)
        {
            // check to see if we want all the blocks/piceces
            bool all_requested = true;
            for (const auto& block : m_reqd)
            {
                for (auto piece : block)
                {
                    all_requested &= piece;
                }
            }

            if (all_requested)
            {
                // since we want everything then we will copy everything over to the requested
                m_reqd = m_recvd;
            }

            const auto blk_idx = blk.begin / torrent::BLOCK_LEN;
            return !m_reqd[blk.piece_idx][blk_idx];
        }

        bool is_done() const
        {
            bool all = true;
            for (const auto& block : m_recvd)
            {
                for (auto piece : block)
                {
                    all &= piece;
                }
            }

            return all;
        }

        void write_percent_recvd(std::ostream& out) const 
        { 
            size_t recvd = 0;

            for (auto& blocks : m_recvd)
            {
                size_t i = 0;
                for (auto b : blocks)
                {
                    if (b)
                        i++;
                }

                recvd += i;
            }

            size_t total = 0;
            for (auto& blocks : m_recvd)
                recvd += blocks.size();

            const auto percent = floor(recvd / total * 100);
            out << "progress: " << percent;
        }

    private:
        std::vector<std::vector<bool>> m_reqd;
        std::vector<std::vector<bool>> m_recvd;

        inline static std::vector<std::vector<bool>> make_piece_array(torrent_handle tor)
        {
            const size_t pcs = tor->pieces().size() / 20;
            std::vector<std::vector<bool>> result(pcs);
            for (size_t i = 0; i < result.size(); i++)
                result[i] = std::vector<bool>(tor->blocks_per_pc(i), false);
        }
    };
} // namespace storrent