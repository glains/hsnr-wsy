#ifndef HSNR_WSY_4INAROW_GAME_H
#define HSNR_WSY_4INAROW_GAME_H

#include <cmath>
#include <bitset>
#include <vector>
#include <array>
#include <ostream>
#include <sstream>

typedef unsigned long long ull;

class Board {
public:
    constexpr const static int ROWS = 6;
    constexpr const static int COLS = 7;
    constexpr const static int N = ROWS * COLS;

    Board() {
        _nmves = 0;
        _mves0 = 0;
        _mves1 = 0;
        _plyr = &_mves0;
        _h = {4, 4, 4, 4, 4, 4, 4, 3};
    }

    Board(ull mves0, ull mves1) {
        _nmves = 12;
        _mves0 = mves0;
        _mves1 = mves1;
        _plyr = &_mves0;
        _h = {4, 4, 4, 4, 4, 4, 4, 3};
    }

    Board(const Board &b) {
        _nmves = b._nmves;
        _mves0 = b._mves0;
        _mves1 = b._mves1;
        _plyr = b._plyr;
        _h = b._h; // TODO: copy
    }

    //-----------------------------------------------------------------------

    [[nodiscard]]
    int move() const;

    [[nodiscard]]
    Board move(int col) const;

    [[nodiscard]]
    inline int lastMove() const {
        return _h[COLS];
    }

    [[nodiscard]]
    bool won() const;

    int testScore();

    //-----------------------------------------------------------------------

    /*
    friend std::ostream &operator<<(std::ostream &os, const Board &b) {
        return os << "moves: " << b.moves() << std::endl
                  << "mask : " << b.mask();
    }
    */

private:
    // total amount of moves made
    short _nmves;
    // encoded by cols of size N, since its faster to
    // generate moves
    ull _mves0;
    ull _mves1;
    ull *_plyr;
    // height of cols
    std::array<short, COLS + 1> _h;

    //-----------------------------------------------------------------------

    [[nodiscard]]
    inline std::vector<Board> nextMoves() const;

    /**
     * Validates if the current contains a winning position.
     *
     * @param lastCol the last column a move has been made
     * @return true if the current player has 4-in-a-row
     */
    inline bool won(int lastCol) const;

    [[nodiscard]]
    int ab_max(int depth, int a, int b) const;

    [[nodiscard]]
    int ab_min(int depth, int a, int b) const;

    [[nodiscard]]
    int ab_score() const;

};

template<typename T>
std::string to_string(const T &val) {
    std::ostringstream ss;
    ss << val;
    return ss.str();
}

#endif //HSNR_WSY_4INAROW_GAME_H
