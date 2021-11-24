#include "game.h"

#include <set>
#include <iostream>

using namespace std;

typedef unsigned long long ull;

const ull M = 0xf;
const ull R1 = 0x0f;
const ull R2 = 0x1e;
const ull R3 = 0x3c;

const ull DM = 0x0204081ull;

const array<ull, Board::ROWS> ROW_AND = {
        0x01041041041,
        0x02082082082,
        0x04104104104,
        0x08208208208,
        0x10410410410,
        0x20820820820
};

const ull DIAG_MASK = 0x204081;

const array<ull, Board::ROWS> DIA_AND = {
        0b00000000000000000000000100000010000001000000100,
        0b00000000000000000100000010000001000000100000010,
        0b00000000000100000010000001000000100000010000001,
        0b00000100000010000001000000100000010000001000000,
        0b01000000100000010000001000000100000000000000000,
        0b00100000010000001000000100000000000000000000000,
};

array<ull, Board::ROWS> setupMagicRowBits() noexcept {
    array<ull, Board::ROWS> res{};
    auto base = bitset<Board::N>(0x1).to_ullong();
    for (int r = 0; r < Board::ROWS; ++r) {
        auto cur = base, magic = base;
        for (int i = 0; i < Board::COLS; ++i) {
            cur <<= Board::COLS;
            magic += cur;
        }
        bitset<Board::N> ts(cur);
        //cout << ts;
        res[r] = magic;
        base <<= 1;
    }

    return res;
}

array<ull, Board::ROWS> ROW_MAG = setupMagicRowBits();

array<ull, Board::COLS> setupMagicDiaBits() noexcept {
    array<ull, Board::COLS> res{};
    auto base = bitset<Board::N>(0x1 << (Board::N - 3 * 7)).to_ullong();
    for (int c = 0; c < 1; ++c) {
        auto cur = base, magic = base;
        for (int i = 0; i < 3; ++i) {
            cur <<= 7; // diag-distance
            magic += cur;
        }
        res[c] = magic;
        base <<= 1;
    }

    return res;
}

array<ull, Board::COLS> DIA_MAG = setupMagicDiaBits();

//-----------------------------------------------------------------------

vector<Board> Board::nextMoves() const {
    vector<Board> res;
    if (_plyr) {
        for (int c = 0; c < COLS; ++c) {
            if (_h[c] < ROWS) {
                Board b(*this);
                int off = c * ROWS * _h[c];
                b._mves1 |= 1 << off;
                res.push_back(b);
            }
        }
    } else {
        for (int c = 0; c < COLS; ++c) {
            if (_h[c] < ROWS) {
                Board b(*this);
                int off = c * ROWS * _h[c];
                b._mves0 |= 1 << off;
                res.push_back(b);
            }
        }
    }
    return res;
}

bool Board::won() {
    const ull t = _plyr ? _mves1 : _mves0;
    // rows
    for (int r = 0; r < ROWS; ++r) {
        ull row = (t & ROW_AND[r]) * ROW_MAG[r] >> (N - ROWS);
        if ((row & R1) == R1 ||
            (row & R2) == R2 ||
            (row & R3) == R3)
            return true;
    }

    if (_nmves < 8) {
        return false; // min num to win rows
    }
    // cols
    for (int c = 0; c < COLS; ++c) {
        if (_h[c] < 4) continue; // minimum win at height 4
        auto col = (t >> (c * ROWS));
        if ((col & R1) == R1 ||
            (col & R2) == R2 ||
            (col & R3) == R3)
            return true;
    }

    // diag
    if (_nmves < 10) {
        return false; // min num to win diag
    }

    /*
    // left bottom to right top column
    const int maxDiag = 5;
    for (int c = 0; c < maxDiag; ++c) {
        if (_h[c + 3] < 4) continue;
        const int off = ROWS * c;
        // left bottom to right top, length 4
        if ((t >> (0 + off) & DIAG_MASK) == DIAG_MASK ||
            (t >> (1 + off) & DIAG_MASK) == DIAG_MASK ||
            (t >> (2 + off) & DIAG_MASK) == DIAG_MASK) {
            return true;
        }
    }
     */

    return false;
}
