#include "game.h"

#include <limits>
#include <iostream>

#define DEBUG
#undef  DEBUG

#define OUT_BIT(long) (cout << bitset<N>(long) << endl);
#define BIT_CNT(l) (bitset<N>(l).count())

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

const int SCORE_WIN = 50000;

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
    for (int col = 0; col < COLS; ++col) {
        if (_h[col] < ROWS) {
            int off = col * ROWS + _h[col];
            Board b(
                    _nmves + 1,
                    _mves0 | (_toMove * (1ull << off)),
                    _mves1 | (!_toMove * (1ull << off)),
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

bool Board::end() const {
    if (_nmves == N) {
        return true;
    }
    return won(lastMove());

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
    // check if the other player has end
    ull t = _toMove ? _mves1 : _mves0;
    bool win = false;
    // rows
    int prevHeight = _h[lastCol] - 1;
    ull row = ((t >> prevHeight) & ROW_MASK);
    win |= (row & R1) == R1 ||
           (row & R2) == R2 ||
           (row & R3) == R3 ||
           (row & R4) == R4;
    if (win) return true;

    // cols
    ull col = (t >> (lastCol * ROWS));
    win |= (col & C1) == C1 ||
           (col & C2) == C2 ||
           (col & C3) == C3;
    if (win) return true;

    /*
    const int off = ROWS * lastCol;
    // left bottom to right top, length 4
    win |= (t >> (0 + off) & DIA_MASK) == DIA_MASK ||
           (t >> (1 + off) & DIA_MASK) == DIA_MASK ||
           (t >> (2 + off) & DIA_MASK) == DIA_MASK;
    */
    if (win) return true;
    return false;
}

Move Board::search(int depth) const {
    return ab_max(depth, INT_MIN, INT_MAX);
}

Board Board::move(int col) const {
    if (won(lastMove())) {
        throw std::logic_error("already end");
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
    if (depth == 0) {
        if (won(lastMove())) {
            return {.col=lastMove(), .score=-SCORE_WIN};
        }
        return {.col=lastMove(), .score=ab_score()};
    }
    // checking for a win here saves a call in #nextMoves and #score
    if (won(lastMove())) {
        return {.col=lastMove(), .score=-SCORE_WIN};
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
    if (depth == 0) {
        if (won(lastMove())) {
            return {.col=lastMove(), .score=SCORE_WIN};
        }
        return {.col=lastMove(), .score=ab_score()};
    }
    // checking for a win here saves a call in #nextMoves and #score
    if (won(lastMove())) {
        return {.col=lastMove(), .score=SCORE_WIN};
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
    int s1 = scorePlyr(true);
    int s2 = scorePlyr(false);
    return s1 - s2;
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

    // compute the global maximum coverage score
    // for each spot of 4 pins (row, col, dia)
    //  - compute positive z if all pins not taken by p2, else 0
    //  - multiply z with amount of pins already occupied by p1
    //    - will be zero if no pin occupied
    ull cov = 0;
    ull fill = 0;

    // rows
    for (int row = 0; row < ROWS; ++row) {
        int shift = row;
        ull row1 = (t1 >> shift);
        ull row2 = (t2 >> shift);

        bool m1 = ((row2 & R1) == 0) * ((row1 & R1) > 0);
        cov |= (m1 * R1) << shift;
        fill += m1 * BIT_CNT(row1 & R1);

        bool m2 = ((row2 & R2) == 0) * ((row1 & R2) > 0);
        cov |= (m2 * R2) << shift;
        fill += m2 * BIT_CNT(row1 & R2);

        bool m3 = ((row2 & R3) == 0) * ((row1 & R3) > 0);
        cov |= (m3 * R3) << shift;
        fill += m3 * BIT_CNT(row1 & R3);

        bool m4 = ((row2 & R4) == 0) * ((row1 & R4) > 0);
        cov |= (m4 * (row1 | R4)) << shift;
        fill += m4 * BIT_CNT(row1 & R4);
    }

    // cols
    for (int col = 0; col < COLS; ++col) {
        int shift = col * ROWS;
        auto col1 = (t1 >> shift);
        auto col2 = (t2 >> shift);

        bool m1 = ((col2 & C1) == 0) * ((col1 & C1) > 0);
        cov |= (m1 * C1) << shift;
        fill += m1 * BIT_CNT(col1 & C1);

        bool m2 = ((col2 & C2) == 0) * ((col1 & C2) > 0);
        cov |= (m2 * C2) << shift;
        fill += m2 * BIT_CNT(col1 & C2);

        bool m3 = ((col2 & C3) == 0) * ((col1 & C3) > 0);
        cov |= (m3 * C3) << shift;
        fill += m3 * BIT_CNT(col1 & C3);
    }

    // diag

    return (int) (2 * BIT_CNT(cov) + fill);
}
