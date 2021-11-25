#include "game.h"

#include <limits>
#include <iostream>

#define DEBUG
#undef  DEBUG
#define OUT_BIT(long) (cout << bitset<N>(long) << endl);

using namespace std;

typedef unsigned long long ull;

const ull M = 0xf;
const ull R1 = 0x41041;
const ull R2 = 0x1041040;
const ull R3 = 0x41041000;
const ull R4 = 0x1041040000;

const ull C1 = 0x0f;
const ull C2 = 0x1e;
const ull C3 = 0x3c;

const ull DM = 0x0204081ull;

const array<ull, Board::ROWS> ROW_MSK = {
        0x01041041041,
        0x02082082082,
        0x04104104104,
        0x08208208208,
        0x10410410410,
        0x20820820820
};

const ull DIA_MASK = 0x204081;
const ull ROW_MASK = 0x01041041041;
const ull COL1_MASK = 0x3f;

const array<ull, Board::ROWS> DIA_AND = {
        0b00000000000000000000000100000010000001000000100,
        0b00000000000000000100000010000001000000100000010,
        0b00000000000100000010000001000000100000010000001,
        0b00000100000010000001000000100000010000001000000,
        0b01000000100000010000001000000100000000000000000,
        0b00100000010000001000000100000000000000000000000,
};

//-----------------------------------------------------------------------

int Board::colRnk(int idx) const {
    auto col = (_mves0 >> (idx * ROWS)) | (_mves1 >> (idx * ROWS));
    return (int) bitset<N>(col & COL1_MASK).count();
}

vector<Board> Board::nextMoves() const {
    vector<Board> res;
    if (won(lastMove())) {
        return res;
    }

    ull p1;
    ull p2;
    if (_toMove) {
        p1 = _mves0;
        p2 = _mves1;
    } else {
        p1 = _mves1;
        p2 = _mves0;
    }

    for (int col = 0; col < COLS; ++col) {
        if (_h[col] < ROWS) {
            int off = col * ROWS + _h[col];
            Board b(
                    _nmves + 1,
                    p1 | (1ull << off),
                    p2,
                    !_toMove,
                    _h
            );
            b._h[col]++;
            b._h[COLS] = col;
            res.push_back(b);
        }
    }
    return res;
}

bool Board::won() const {
    bool win = false;
    // check if the other player (last move) has won
    const ull t = _toMove ? _mves1 : _mves0;

    // rows
    const int maxRow = 5;
    for (int r = 0; r < maxRow; ++r) {
        ull row = (t & ROW_MSK[r]);
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
    // check if the other player has won
    ull t = _toMove ? _mves1 : _mves0;
    bool win = false;
    // rows
    ull row = (t & ROW_MSK[_h[lastCol]]);
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

Move Board::search(int depth) const {
    return ab_max(depth, INT_MIN, INT_MAX);
}

Board Board::move(int col) const {
    if (won(lastMove())) {
        throw std::logic_error("already won");
    }
    if (_toMove) {
        if (_h[col] < ROWS) {
            int off = col * ROWS + _h[col];
            Board b(
                    _nmves + 1,
                    _mves0 | (1ull << off),
                    _mves1,
                    !_toMove,
                    _h
            );
            b._h[col]++;
            b._h[COLS] = col;

            return b;
        }
        throw std::logic_error("already full");
    } else {
        if (_h[col] < ROWS) {
            int off = col * ROWS + _h[col];
            Board b(
                    _nmves + 1,
                    _mves0,
                    _mves1 | (1ull << off),
                    !_toMove,
                    _h
            );
            b._h[col]++;
            b._h[COLS] = col;

            return b;
        }
        throw std::logic_error("already full");
    }
}

//-----------------------------------------------------------------------
// Alpha-Beta-Pruning

inline Move Board::ab_max(int depth, int a, int b) const {
    if (depth == 0 || _nmves == N) {
        return {.col=lastMove(), .score=ab_score()};
    }
    vector<Board> moves = nextMoves();
    if (moves.empty()) {
        return {.col=lastMove(), .score=ab_score()};
    }

    int col = 0;
    int max = a;
    for (auto &m: moves) {
        Move val = m.ab_min(depth - 1, max, b);
        if (val.score > max) {
            col = m.lastMove();
            max = val.score;
            if (max >= b) break; // cutoff
        }
    }
    return {.col = col, .score = max};
}

inline Move Board::ab_min(int depth, int a, int b) const {
    if (depth == 0 || _nmves == N) {
        return {.col=lastMove(), .score=ab_score()};
    }
    vector<Board> moves = nextMoves();
    if (moves.empty()) {
        return {.col=lastMove(), .score=ab_score()};
    }

    int col = 0;
    int min = b;
    for (auto &m: moves) {
        Move val = m.ab_max(depth - 1, a, min);

#ifdef DEBUG
        cout << "col: " << val.col << " ; score: " << val.score << endl;
#endif

        if (val.score < min) {
            col = m.lastMove();
            min = val.score;
            if (min <= a) break; // cutoff
        }
    }
    return {.col = col, .score = min};
}

int Board::testScore() {
    return ab_score();
}

inline int Board::ab_score() const {
    return scorePlyr(true) - scorePlyr(false);

}

inline int Board::scorePlyr(bool plyr) const {
    ull t1;
    ull t2;
    if (plyr) {
        t1 = _mves0;
        t2 = _mves1;
    } else {
        t1 = _mves1;
        t2 = _mves0;
    }

    int lastCol = lastMove();
    if (won(lastCol)) {
        return (_toMove) ? -5000 : 5000;
    }

    // compute the global maximum coverage score
    // for each spot of 4 pins (row, col, dia)
    //  - compute positive z if all pins not taken by p2, else 0
    //  - multiply z with amount of pins already occupied by p1
    //    - will be zero if no pin occupied
    ull cov = 0;

    // rows
    for (int row = 0; row < ROWS; ++row) {
        int shift = row;
        ull row1 = (t1 >> shift);
        ull row2 = (t2 >> shift);
        cov |= ((row2 & R1) == 0) * ((row1 & R1) > 1) * (row1 | R1);
        cov |= ((row2 & R2) == 0) * ((row1 & R2) > 1) * (row1 | R2);
        cov |= ((row2 & R3) == 0) * ((row1 & R3) > 1) * (row1 | R3);
        cov |= ((row2 & R4) == 0) * ((row1 & R4) > 1) * (row1 | R4);
    }

    // cols
    for (int col = 0; col < COLS; ++col) {
        int shift = col * ROWS;
        auto col1 = (t1 >> shift);
        auto col2 = (t2 >> shift);
        cov |= ((col2 & C1) == 0) * ((col1 & C1) > 1) * (col1 | C1);
        cov |= ((col2 & C2) == 0) * ((col1 & C2) > 1) * (col1 | C2);
        cov |= ((col2 & C3) == 0) * ((col1 & C3) > 1) * (col1 | C3);
    }

    // diag

    return (int) bitset<N>(cov).count(); // optimize with raw count from <bits>
}
