#include "game.h"

#include <limits>
#include <iostream>

#define DEBUG
#undef  DEBUG

#define OUT_BIT(long) (cout << bitset<N>(long) << endl);
#define BIT_CNT(l) (bitset<N>(l).count())

using namespace std;

typedef unsigned long long ull;


//-----------------------------------------------------------------------
// MASKS

const ull M = 0xf;
// whole board set
const ull ALL = 0x3FFFFFFFFFF;

const ull R1 = 0x41041;
const ull R2 = 0x1041040;
const ull R3 = 0x41041000;
const ull R4 = 0x1041040000;

// 3-connected
const ull R_CON3_1 = 0x41041;
const ull R_CON3_2 = 0x1041040;
const ull R_CON3_3 = 0x41041000;
const ull R_CON3_4 = 0x1041040000;

const ull C1 = 0x0f;
const ull C2 = 0x1e;
const ull C3 = 0x3c;
const ull C_ALL_4 = 0xFFFFFF;

// diagonals bot-left to top-right
const ull D1 = 0x810204;
const ull D2 = 0x20408102;
const ull D3 = 0x810204081;
const ull D4 = 0x20408102040;
const ull D5 = 0x10204081000;
const ull D6 = 0x8102040000;

const ull D_MSK[Board::N] = {
        D3, D2, D1, 0, 0, 0,
        D4, D3, D2, D1, 0, 0,
        D5, D4, D3, D2, D1, 0,
        D6, D5, D4, D3, D2, D1,
        0, D6, D5, D4, D3, D2,
        0, 0, D6, D5, D4, D3,
        0, 0, 0, D6, D5, D4
};

// masks used to block columns 2,4 for a diagonal check
const ull D_BLKR_C2 = (0xFFFull << (Board::ROWS * 2)) ^ ALL;
const ull D_BLKR_C4 = (0xFFFull << (Board::ROWS * 4)) ^ ALL;

const ull DIA_MASK = 0x204081;
const ull ROW_MASK = 0x01041041041;
const ull COL1_MASK = 0x3f;

// score weights
const int SCORE_2 = 2;
const int SCORE_3 = 4;
const int SCORE_WIN = 50000;

// assume that moves generated in the middle of the board have
// the most impact on the game, therefore they should be
// evaluated in that order
const int COL_ORD[7] = {3, 2, 4, 1, 5, 0, 6};

//-----------------------------------------------------------------------

int Board::colRnk(int idx) const {
    auto col = (_mves0 >> (idx * ROWS)) | (_mves1 >> (idx * ROWS));
    return (int) bitset<N>(col & COL1_MASK).count();
}

vector<Board> Board::nextMoves() const {
    vector<Board> res;
    for (int col: COL_ORD) {
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
}

inline bool Board::wonDia(ull t, int col) {
    const int off = ROWS * col;
    return BIT_CNT((t & D_MSK[off]) & (C_ALL_4 << (off + 0))) == 4 ||
           BIT_CNT((t & D_MSK[off]) & (C_ALL_4 << (off + 1))) == 4 ||
           BIT_CNT((t & D_MSK[off]) & (C_ALL_4 << (off + 2))) == 4 ||
           BIT_CNT((t & D_MSK[off]) & (C_ALL_4 << (off + 3))) == 4;
}

bool Board::won(int lastCol) const {
    // check if the other player has won with his last move
    ull t = _toMove ? _mves1 : _mves0;

    bool won = false;
    // rows
    int prevHeight = _h[lastCol] - 1;
    ull row = ((t >> prevHeight) & ROW_MASK);
    won |= (row & R1) == R1 ||
           (row & R2) == R2 ||
           (row & R3) == R3 ||
           (row & R4) == R4;
    if (won) return true;

    // cols
    ull col = (t >> (lastCol * ROWS));
    won |= (col & C1) == C1 ||
           (col & C2) == C2 ||
           (col & C3) == C3;
    if (won) return true;

    // dias
    won |= wonDia(t, lastCol) ||
           wonDia(invert(t), N - lastCol);
    if (won) return true;

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

    int rfill[5];
    // rows
    for (int row = 0; row < ROWS; ++row) {
        int shift = row;
        ull row1 = (t1 >> shift);
        ull row2 = (t2 >> shift);

        bool m1 = ((row2 & R1) == 0) * ((row1 & R1) > 0);
        cov |= (m1 * R1) << shift;
        rfill[m1 * BIT_CNT(row1 & R1)]++;

        bool m2 = ((row2 & R2) == 0) * ((row1 & R2) > 0);
        cov |= (m2 * R2) << shift;
        rfill[m2 * BIT_CNT(row1 & R2)]++;

        bool m3 = ((row2 & R3) == 0) * ((row1 & R3) > 0);
        cov |= (m3 * R3) << shift;
        rfill[m3 * BIT_CNT(row1 & R3)]++;

        bool m4 = ((row2 & R4) == 0) * ((row1 & R4) > 0);
        cov |= (m4 * (row1 | R4)) << shift;
        rfill[m4 * BIT_CNT(row1 & R4)]++;
    }

    // cols
    int colFill[5];
    for (int col = 0; col < COLS; ++col) {
        int shift = col * ROWS;
        auto col1 = (t1 >> shift);
        auto col2 = (t2 >> shift);

        bool m1 = ((col2 & C1) == 0) * ((col1 & C1) > 0);
        cov |= (m1 * C1) << shift;
        colFill[m1 * BIT_CNT(col1 & C1)];

        bool m2 = ((col2 & C2) == 0) * ((col1 & C2) > 0);
        cov |= (m2 * C2) << shift;
        colFill[m2 * BIT_CNT(col1 & C2)];

        bool m3 = ((col2 & C3) == 0) * ((col1 & C3) > 0);
        cov |= (m3 * C3) << shift;
        colFill[m3 * BIT_CNT(col1 & C3)];
    }

    // diag

    // by starting at idx 2, with a constant offset of 6,
    // each iteration a different diag is visited
    for (int i = 2; i < N - ROWS; i += 6) {
        auto dia1 = (t1 & D_MSK[i]);
        auto dia2 = (t2 & D_MSK[i]);

        ull blk1 = C_ALL_4 << 0 * ROWS;
        ull local1 = dia1 & blk1;
        bool m1 = ((dia2 & blk1) == 0) * (local1 > 0);
        // wrong: we need to iter over all diags len 4
        cov |= (m1 * local1);

        cout << *this << endl;
        cout << bits_to_string(local1) << endl;
        cout << bits_to_string(cov) << endl;

        ull blk2 = C_ALL_4 << 1 * ROWS;
        ull local2 = dia1 & blk2;
        bool m2 = ((dia2 & blk2) == 0) * (local2 > 0);
        cov |= (m2 * local2);

        ull blk3 = C_ALL_4 << 2 * ROWS;
        ull local3 = dia1 & blk3;
        bool m3 = ((dia2 & blk3) == 0) * (local3 > 0);
        cov |= (m3 * local3);

        ull blk4 = C_ALL_4 << 3 * ROWS;
        ull local4 = dia1 & blk4;
        bool m4 = ((dia2 & blk4) == 0) * (local4 > 0);
        cov |= (m4 * local4);
    }

    cout << *this << endl;
    cout << bits_to_string(cov) << endl;

    int totalFill = 0;
    return (int) (2 * BIT_CNT(cov) + totalFill);
}

ull Board::invert(ull l) const {
    return l; // TODO
}
