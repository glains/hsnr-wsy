#include "game.h"

#include <iostream>

#define OUT_BIT(long) (cout << bitset<N>(long) << endl);

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

const ull DIA_MASK = 0x204081;
const ull ROW_MASK = 0x01041041041;

const array<ull, Board::ROWS> DIA_AND = {
        0b00000000000000000000000100000010000001000000100,
        0b00000000000000000100000010000001000000100000010,
        0b00000000000100000010000001000000100000010000001,
        0b00000100000010000001000000100000010000001000000,
        0b01000000100000010000001000000100000000000000000,
        0b00100000010000001000000100000000000000000000000,
};

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

bool Board::won() const {
    bool win = false;
    const ull t = _plyr ? _mves1 : _mves0;

    // rows
    const int maxRow = 5;
    for (int r = 0; r < maxRow; ++r) {
        ull row = (t & ROW_AND[r]);
        win |= (row & R1) == R1 ||
               (row & R2) == R2 ||
               (row & R3) == R3;
    }
    if (win) return true;

    // cols
    for (int c = 0; c < COLS; ++c) {
        auto col = (t >> (c * ROWS));
        win |= (col & R1) == R1 ||
               (col & R2) == R2 ||
               (col & R3) == R3;
    }
    if (win) return true;

    /*
    // diag
    if (_nmves < 10) {
        return false; // min num to win diag
    }

    // left bottom to right top column
    const int maxDiag = 5;
    for (int c = 0; c < maxDiag; ++c) {
        if (_h[c + 3] < 4) continue;
        const int off = ROWS * c;
        // left bottom to right top, length 4
        if ((t >> (0 + off) & DIA_MASK) == DIA_MASK ||
            (t >> (1 + off) & DIA_MASK) == DIA_MASK ||
            (t >> (2 + off) & DIA_MASK) == DIA_MASK) {
            return true;
        }
    }
    */
    return false;
}

bool Board::won(int lastCol) const {
    ull t = *_plyr;
    bool win = false;
    // rows
    ull row = (t & ROW_AND[_h[lastCol]]);
    win |= (row & R1) == R1 ||
           (row & R2) == R2 ||
           (row & R3) == R3;
    if (win) return true;

    // cols
    ull col = (t >> (lastCol * ROWS));
    win |= (col & R1) == R1 ||
           (col & R2) == R2 ||
           (col & R3) == R3;
    if (win) return true;

    const int off = ROWS * lastCol;
    // left bottom to right top, length 4
    win |= (t >> (0 + off) & DIA_MASK) == DIA_MASK ||
           (t >> (1 + off) & DIA_MASK) == DIA_MASK ||
           (t >> (2 + off) & DIA_MASK) == DIA_MASK;

    if (win) return true;
    return false;
}

int Board::move() const {
    return 0;
}

Board Board::move(int col) const {
    return *this;
}

//-----------------------------------------------------------------------
// Alpha-Beta-Pruning

inline int Board::ab_max(int depth, int a, int b) const {
    if (depth == 0 || _nmves == N) {
        return ab_score();
    }
    int max = a;
    vector<Board> moves = nextMoves();
    for (auto &m: moves) {
        int val = m.ab_min(depth - 1, max, b);
        if (val > max) {
            max = val;
            if (max >= b) break; // cutoff
        }
    }
    return max;
}

inline int Board::ab_min(int depth, int a, int b) const {
    if (depth == 0 || _nmves == N) {
        return ab_score();
    }
    int min = b;
    vector<Board> moves = nextMoves();
    for (auto &m: moves) {
        int val = m.ab_max(depth - 1, a, min);
        if (val < min) {
            min = val;
            if (min <= a) break; // cutoff
        }
    }
    return min;
}

int Board::testScore() {
    return ab_score();
}

inline int Board::ab_score() const {
    ull t = *_plyr;
    ull t2 = (&_mves0 == _plyr) ? _mves1 : _mves0;
    int lastCol = lastMove();
    if (won(lastCol)) {
        return (&_mves0 == _plyr) ? 5000 : -5000;
    }
    ull all = t | t2;

    // compute the global maximum coverage score
    // for each spot of 4 pins (row, col, dia)
    //  - compute positive z if all pins not taken by p2, else 0
    //  - multiply z with amount of pins already occupied by p1
    //    - will be zero if no pin occupied

    ull cov = t;
    // rows

    // cols
    int local = 0;
    for (int c = 0; c < COLS; ++c) {
        int shift = c * ROWS;
        auto col = (t >> shift);
        auto col2 = (t2 >> shift);
        cov |= ((((col2 ^ R1) & R1) == R1) * ((col & R1) > 1) * (col ^ R1)) << shift;
        cov |= ((((col2 ^ R2) & R2) == R2) * ((col & R2) > 1) * (col ^ R2)) << shift;
        cov |= ((((col2 ^ R3) & R3) == R3) * ((col & R3) > 1) * (col ^ R3)) << shift;
    }

    // diag

    return (int) bitset<N>(cov).count();
}
