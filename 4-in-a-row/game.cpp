#include "game.h"

#include <bit>
#include <cmath>
#include <limits>
#include <iostream>
#include <algorithm>

#define DEBUG
#undef  DEBUG

#define OUT_BIT(long) (cout << bitset<N>(long) << endl);

#define AB_MIN std::numeric_limits<int>::min()
#define AB_MAX std::numeric_limits<int>::max()

using namespace std;

typedef unsigned long long ull;

//-----------------------------------------------------------------------
// MASKS

const ull R1 = 0x41041;
const ull R2 = 0x1041040;
const ull R3 = 0x41041000;
const ull R4 = 0x1041040000;

// 3-connected
const ull R_S1_1 = 0x1;
const ull R_S1_2 = 0x40;
const ull R_S1_3 = 0x1000;
const ull R_S1_4 = 0x40000;

// 3-connected
const ull R_CON3_1 = 0x41041;
const ull R_CON3_2 = 0x1041040;
const ull R_CON3_3 = 0x41041000;
const ull R_CON3_4 = 0x1041040000;

const ull C1 = 0x0f;
const ull C2 = 0x1e;
const ull C3 = 0x3c;

const ull C_ALL4_1 = 0xFFFFFF;
const ull C_ALL4_2 = 0x3FFFFFC0;
const ull C_ALL4_3 = 0xFFFFFF000;
const ull C_ALL4_4 = 0x3FFFFFC0000;

// diagonals bot-left to top-right
const ull D11 = 0x810204;
const ull D12 = 0x20408102;
const ull D13 = 0x810204081;
const ull D14 = 0x20408102040;
const ull D15 = 0x10204081000;
const ull D16 = 0x8102040000;
// diagonals bot-left to top-right
const ull D21 = 0x42108;
const ull D22 = 0x1084210;
const ull D23 = 0x42108420;
const ull D24 = 0x1084210800;
const ull D25 = 0x2108420000;
const ull D26 = 0x4210800000;

const ull D_MSK_0 = 0x204081;
const ull D_MSK_1 = 0x8888;

const ull D1_MSK[Board::N] = {
        D13, D12, D11, 0, 0, 0,
        D14, D13, D12, D11, 0, 0,
        D15, D14, D13, D12, D11, 0,
        D16, D15, D14, D13, D12, D11,
        0, D16, D15, D14, D13, D12,
        0, 0, D16, D15, D14, D13,
        0, 0, 0, D16, D15, D14
};
const ull D2_MSK[Board::N] = {
        0, 0, 0, D21, D22, D23,
        0, 0, D21, D22, D23, D24,
        0, D21, D22, D23, D24, D25,
        D21, D22, D23, D24, D25, D26,
        D22, D23, D24, D25, D26, 0,
        D23, D24, D25, D26, 0, 0,
        D24, D25, D26, 0, 0, 0
};

const ull ROW_MASK = 0x01041041041;
const ull COL1_MASK = 0x3f;

// score weights
const int SCORE_2 = 2;
const int SCORE_3 = 4;
const int SCORE_WIN = 500000;
const int SCORE_WINNING = SCORE_WIN - Board::N;

// assume that moves generated in the middle of the board have
// the most impact on the game, therefore they should be
// evaluated in that order
const int COL_ORD[7] = {3, 2, 4, 1, 5, 0, 6};

//-----------------------------------------------------------------------

int Board::colRnk(int idx) const {
    auto col = (_mves0 >> (idx * ROWS)) | (_mves1 >> (idx * ROWS));
    return (int) bitset<N>(col & COL1_MASK).count();
}

inline vector<Board> Board::nextMoves() const {
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

inline vector<Board> Board::nextMovesSorted() const {
    vector<Board> moves = nextMoves();
    int scores[moves.size()];
    for (const auto &m: moves) {
        scores[m.lastMove()] = m.ab_max(3, AB_MIN, AB_MAX).score;
    }
    sort(moves.begin(), moves.end(),
         [&scores](const Board &a, const Board &b) -> bool {
             return scores[a.lastMove()] > scores[b.lastMove()];
         });
    return moves;
}

bool Board::end() const {
    return _nmves == N || won(lastMove());
}

inline bool Board::wonDia(ull t, int col) const {
    const int off = ROWS * col + _h[col] - 1;
    ull blk1 = t & D1_MSK[off];
    ull blk2 = t & D2_MSK[off];
    return popcount(blk1 & C_ALL4_1) == 4 ||
           popcount(blk1 & C_ALL4_2) == 4 ||
           popcount(blk1 & C_ALL4_3) == 4 ||
           popcount(blk1 & C_ALL4_4) == 4 ||
           popcount(blk2 & C_ALL4_1) == 4 ||
           popcount(blk2 & C_ALL4_2) == 4 ||
           popcount(blk2 & C_ALL4_3) == 4 ||
           popcount(blk2 & C_ALL4_4) == 4;
}


inline bool Board::won(int lastCol) const {
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
    won |= wonDia(t, lastCol);
    if (won) return true;

    return false;
}

Eval Board::search() const {
    if (_nmves < 3 * ROWS) {
        return search(11);
    }
    // fully expand tree
    return search(N);
}

Eval Board::search(int depth) const {
    Move move = ab_max_init(depth, AB_MIN, AB_MAX);
    Status status = EQUAL;
    if (move.score >= SCORE_WINNING) {
        status = WINNING;
    } else if (move.score <= -SCORE_WINNING) {
        status = LOSING;
    }
    return {.move = move, .status = status};
}

Board Board::move(int col) const {
    if (won(lastMove())) {
        throw std::invalid_argument("already end");
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
        throw std::invalid_argument("already full");
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
        throw std::invalid_argument("already full");
    }
}

//-----------------------------------------------------------------------
// Alpha-Beta-Pruning

inline Move Board::ab_max_init(int depth, int a, int b) const {
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

    vector<Board> moves = nextMovesSorted();
    if (moves.empty()) {
        return {.col=lastMove(), .score=ab_score()};
    }

    int col = 0;
    int max = a;

    // chose a single non-losing move if it exists
    int c = 0;
    for (auto &m: moves) {
        Move val = m.ab_min(1, a, b);
        if (val.score > -SCORE_WINNING) {
            col = m.lastMove();
            max = val.score;
            c++;
        }
    }
    if (c == 1) {
        return {.col = col, .score = max};
    }

    col = 0;
    max = a;

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

inline Move Board::ab_max(int depth, int a, int b) const {
    if (depth == 0) {
        if (won(lastMove())) {
            return {.col=lastMove(), .score=-SCORE_WIN + _nmves};
        }
        return {.col=lastMove(), .score=ab_score()};
    }
    // checking for a win here saves a call in #nextMoves and #score
    if (won(lastMove())) {
        return {.col=lastMove(), .score=-SCORE_WIN + _nmves};
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
            return {.col=lastMove(), .score=SCORE_WIN - _nmves};
        }
        return {.col=lastMove(), .score=ab_score()};
    }
    // checking for a win here saves a call in #nextMoves and #score
    if (won(lastMove())) {
        // by subtracting the amount of moves made, a win with
        // the least amount of moves is preferred
        return {.col=lastMove(), .score=SCORE_WIN - _nmves};
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

    int mvesLeft = N - _nmves;

    // compute the global maximum covDiag score
    // for each spot of 4 pins (row, col, dia)
    //  - compute positive z if all pins not taken by p2, else 0
    //  - multiply z with amount of pins already occupied by p1
    //    - will be zero if no pin occupied
    ull cov = 0;

    // counts number of times
    int zugzwang = 0;

    int r_fill[5] = {0};
    // rows
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < 4; ++col) {
            int shift = row + ROWS * col;
            ull row1 = (t1 >> shift);
            ull row2 = (t2 >> shift);

            bool m = ((row2 & R1) == 0) * ((row1 & R1) > 0);
            cov |= (m * R1) << shift;
            int cnt = popcount(row1 & R1);
            r_fill[m * cnt]++;

            int free = countr_zero(~(row1 & R1) & R1);
            int h3 = _h[(free / ROWS) + col];
            bool own = ((mvesLeft - ROWS + h3 + row) & 1) != 0;

            if ((m * (cnt == 3 & h3 != row) * own) * mvesLeft < zugzwang) {
                zugzwang = mvesLeft;
            }
        }
    }

    // cols
    int c_fill[5] = {0};
    for (int col = 0; col < COLS; ++col) {
        int shift = col * ROWS;
        auto col1 = (t1 >> shift);
        auto col2 = (t2 >> shift);

        bool m1 = ((col2 & C1) == 0) * ((col1 & C1) > 0);
        cov |= (m1 * C1) << shift;
        c_fill[m1 * popcount(col1 & C1)]++;

        bool m2 = ((col2 & C2) == 0) * ((col1 & C2) > 0);
        cov |= (m2 * C2) << shift;
        c_fill[m2 * popcount(col1 & C2)]++;

        bool m3 = ((col2 & C3) == 0) * ((col1 & C3) > 0);
        cov |= (m3 * C3) << shift;
        c_fill[m3 * popcount(col1 & C3)]++;
    }

    // diag

    for (int i = 0; i < COLS; ++i) {
        for (int j = 0; j < 3; ++j) {
            int off = i * ROWS + j;

            ull t1_h = t1 >> off;
            ull t2_h = t2 >> off;
            // bottom-left to top-right
            ull dia11 = (t1_h & D_MSK_0);
            ull dia12 = (t2_h & D_MSK_0);
            // top-left to bottom-right
            ull dia13 = (t1_h & D_MSK_1);
            ull dia14 = (t2_h & D_MSK_1);

            bool m11 = ((dia12 & D_MSK_0) == 0) * ((dia11 & D_MSK_0) > 0);
            bool m12 = ((dia14 & D_MSK_1) == 0) * ((dia13 & D_MSK_1) > 0);
            cov |= (m11 * (D_MSK_0 << off));
            cov |= (m12 * (D_MSK_1 << off));

            size_t cnt1 = popcount(dia11 & D_MSK_0);
            size_t cnt2 = popcount(dia13 & D_MSK_1);

            int h31 = _h[countr_zero(~(dia11 & D_MSK_0) & D_MSK_0) / ROWS];
            int h32 = _h[countr_zero(~(dia13 & D_MSK_1) & D_MSK_1) / ROWS];
            zugzwang += (m11 * (cnt1 == 3) * ((mvesLeft + h31) & 1)) * mvesLeft;
            zugzwang += (m12 * (cnt2 == 3) * ((mvesLeft + h32) & 1)) * mvesLeft;
        }
    }

    //int totalFill = r_fill[2] * 2 + r_fill[3] * 4 + c_fill[3] * 10;
    int scoreCov = popcount(cov) * 100;
    int scoreZugzwang = (zugzwang > 0) * _nmves;

    int score = (int) (scoreCov + scoreZugzwang);
    if (score > SCORE_WIN || score < -SCORE_WIN) {
        cout << "WARN: ambiguous score out of bounds" << endl;
    }
    return score;
}

bool Board::won() const {
    return won(lastMove());
}
