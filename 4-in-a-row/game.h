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
        _plyr = true;
        _h = {4, 4, 4, 4, 4, 4, 4};
    }

    Board(ull mves0, ull mves1) {
        _nmves = 12;
        _mves0 = mves0;
        _mves1 = mves1;
        _plyr = true;
        _h = {4, 4, 4, 4, 4, 4, 4};
    }

    Board(const Board &b) {
        _nmves = b._nmves;
        _mves0 = b._mves0;
        _mves1 = b._mves1;
        _plyr = b._plyr;
        _h = b._h; // TODO: copy
    }

    [[nodiscard]]
    inline std::bitset<N> moves() const {
        return _mves0;
    }

    [[nodiscard]]
    inline std::bitset<N> mask() const {
        return _mves1;
    }

    //-----------------------------------------------------------------------

    [[nodiscard]]
    inline std::vector<Board> nextMoves() const;

    bool won();

    //-----------------------------------------------------------------------

    friend std::ostream &operator<<(std::ostream &os, const Board &b) {
        return os << "moves: " << b.moves() << std::endl
                  << "mask : " << b.mask();
    }

private:
    // total amount of moves made
    short _nmves;
    // encoded by cols of size N, since its faster to
    // generate moves
    ull _mves0;
    ull _mves1;
    bool _plyr;
    // height of cols
    std::array<short, COLS> _h;

    //-----------------------------------------------------------------------

};

template<typename T>
std::string to_string(const T &val) {
    std::ostringstream ss;
    ss << val;
    return ss.str();
}

#endif //HSNR_WSY_4INAROW_GAME_H
