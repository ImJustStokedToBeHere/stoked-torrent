#pragma once
#include <cstdint>

namespace storrent
{
    struct piece_block
    {
        piece_block() = default;
        piece_block(const piece_block&) = default;
        piece_block(piece_block&&) = default;

        ~piece_block() = default;

        piece_block& operator=(const piece_block&) = default;
        piece_block& operator=(piece_block&&) = default;

        long long piece_idx{-1};
        size_t begin{0};
        size_t len{0};
    };
}
