#ifndef HSNR_WSY_4INAROW_GAME_H
#define HSNR_WSY_4INAROW_GAME_H

#include <cmath>
#include <bitset>
#include <vector>
#include <array>
#include <ostream>
#include <sstream>
#include <iostream>

typedef unsigned long long ull;

struct Move {
    int col;
    int score;
};

class Board {
public:
    constexpr const static int ROWS = 6;
    constexpr const static int COLS = 7;
    constexpr const static int N = ROWS * COLS;

    Board() : _mves0(0), _mves1(0) {
        _nmves = 0;
        _toMove = true;
        _h = {};
    }

    Board(ull mves0, ull mves1) : _mves0(mves0), _mves1(mves1) {
        _nmves = 12;
        _toMove = true;
        _h = {};
        for (int col = 0; col < COLS; ++col) {
            _h[col] = colRnk(col);
        }
    }

    Board(const Board &b) : _mves0(b._mves0), _mves1(b._mves1) {
        _nmves = b._nmves;
        _toMove = b._toMove;
        _h = b._h; // TODO: copy
    }

    Board(int nmves, const ull mves0, const ull mves1, const bool toMove, const std::array<int, COLS + 1> &h) : _nmves(
            nmves), _mves0(mves0), _mves1(mves1), _toMove(toMove), _h(h) {}

    //-----------------------------------------------------------------------

    [[nodiscard]]
    Move search() const;

    [[nodiscard]]
    Move search(int depth) const;

    Board move(int col) const;

    [[nodiscard]]
    inline int lastMove() const {
        return _h[COLS];
    }

    [[nodiscard]]
    bool end() const;

    [[nodiscard]]
    bool won() const;

    int testScore();

    //-----------------------------------------------------------------------

    friend std::ostream &operator<<(std::ostream &os, const Board &b) {
        auto p1 = std::bitset<N>(b._mves0);
        auto p2 = std::bitset<N>(b._mves1);
        for (int r = ROWS - 1; r >= 0; --r) {
            for (int c = 0; c < COLS; ++c) {
                char ch = p1[c * ROWS + r] ? 'x' : (p2[c * ROWS + r] ? 'o' : '-');
                os << ch << ' ';
            }
            os << std::endl;
        }
        os << std::endl << "0 1 2 3 4 5 6" << std::endl;
        return os;
    }

    static std::string bits_to_string(ull bits) {
        std::stringstream ss;
        auto p1 = std::bitset<N>(bits);
        for (int r = ROWS - 1; r >= 0; --r) {
            for (int c = 0; c < COLS; ++c) {
                char ch = p1[c * ROWS + r] ? 'x' : '-';
                ss << ch << ' ';
            }
            ss << std::endl;
        }
        return ss.str();
    }

private:
    // total amount of moves made
    int _nmves;
    // encoded by cols of size N, since its faster to
    // generate moves
    ull _mves0;
    ull _mves1;
    bool _toMove;
    // height of cols
    // last index stores last played column
    std::array<int, COLS + 1> _h{};

    //-----------------------------------------------------------------------

    std::vector<Board> nextMoves() const;

    std::vector<Board> nextMovesSorted() const;

    [[nodiscard]]
    int colRnk(int idx) const;

    /**
     * Validates if the current contains a winning position.
     *
     * @param lastCol the last column a move has been made
     * @return true if the current player has 4-in-a-row
     */
    bool won(int lastCol) const;

    bool wonDia(ull t, int col) const;

    Move ab_max(int depth, int a, int b) const;

    Move ab_min(int depth, int a, int b) const;

    int ab_score() const;

    int scorePlyr(bool plyr) const;

    // inverts a board column-wise, as if mirrored horizontally
    [[nodiscard]]
    ull invert(ull l) const;

    static void covDiag(ull *cov, int off, ull t1, ull t2);

    Move ab_max_init(int depth, int a, int b) const;
};

template<typename T>
std::string to_string(const T &val) {
    std::ostringstream ss;
    ss << val;
    return ss.str();
}

#endif //HSNR_WSY_4INAROW_GAME_H
