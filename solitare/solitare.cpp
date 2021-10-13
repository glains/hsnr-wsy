
#include <array>
#include <vector>
#include <stack>
#include <iostream>

using namespace std;

class Move {
public:
    inline static const int L = 0;
    inline static const int R = 1;
    inline static const int U = 2;
    inline static const int D = 3;

    int r, c, dir;

    Move(int r, int c, int dir) : r(r), c(c), dir(dir) {}
};

class Board {
public:
    inline static const char SET = '1';
    inline static const char USET = '0';
    inline static const char BLNK = 'x';

    const int _n;
    const int _size;
    char *_arr;
    const pair<short, short> _lastPin;
    int _pins = 0;
    stack<Move> _hist;

    explicit Board(int n, const pair<short, short> lastPin) : _n(n), _size(n * n), _lastPin(lastPin) {
        _arr = new char[_size];
    }

    Board(const Board &o) : _n(o._n), _size(o._size), _lastPin(o._lastPin) {
        _arr = new char[_size];
        std::copy(o._arr, o._arr + _size, _arr);
        _pins = o._pins;
    }

    [[nodiscard]] bool solved() const {
        return _pins == 1 && isSet(_lastPin.first, _lastPin.first);
    }

    void set(int row, int col, char val) {
        _arr[row * _n + col] = val;
        if (val == SET) _pins++;
    }

    [[nodiscard]] inline char &at(int row, int col) const {
        return _arr[row * _n + col];
    }

    [[nodiscard]] inline bool isSet(int row, int col) const {
        return at(row, col) == SET;
    }

    [[nodiscard]] inline bool isFree(int row, int col) const {
        return at(row, col) == USET;
    }

    [[nodiscard]] inline bool canMoveL(int row, int col) const {
        int tc = col - 2;
        return tc >= 0 && isSet(row, col - 1) && isFree(row, tc);
    }

    [[nodiscard]] inline bool canMoveR(int row, int col) const {
        int tc = col + 2;
        return tc < _n && isSet(row, col + 1) && isFree(row, tc);
    }

    [[nodiscard]] inline bool canMoveU(int row, int col) const {
        int tr = row - 2;
        return tr >= 0 && isSet(row - 1, col) && isFree(tr, col);
    }

    [[nodiscard]] inline bool canMoveD(int row, int col) const {
        int tr = row + 2;
        return tr < _n && isSet(row + 1, col) && isFree(tr, col);
    }

    inline void move(const Move &move) {
        at(move.r, move.c) = USET;
        switch (move.dir) {
            case Move::L:
                at(move.r, move.c - 1) = USET;
                at(move.r, move.c - 2) = SET;
                break;
            case Move::R:
                at(move.r, move.c + 1) = USET;
                at(move.r, move.c + 2) = SET;
                break;
            case Move::U:
                at(move.r - 1, move.c) = USET;
                at(move.r - 2, move.c) = SET;
                break;
            case Move::D:
                at(move.r + 1, move.c) = USET;
                at(move.r + 2, move.c) = SET;
                break;
        }
        _pins--;
        _hist.push(move);
    }

    void rewind() {
        Move move = _hist.top();

        at(move.r, move.c) = SET;
        switch (move.dir) {
            case Move::L:
                at(move.r, move.c - 1) = SET;
                at(move.r, move.c - 2) = USET;
                break;
            case Move::R:
                at(move.r, move.c + 1) = SET;
                at(move.r, move.c + 2) = USET;
                break;
            case Move::U:
                at(move.r - 1, move.c) = SET;
                at(move.r - 2, move.c) = USET;
                break;
            case Move::D:
                at(move.r + 1, move.c) = SET;
                at(move.r + 2, move.c) = USET;
                break;
        }
        _pins++;

        _hist.pop();
    }

    [[nodiscard]] vector<Move> moves() const {
        vector<Move> res;
        for (int r = 0; r < _n; ++r) {
            for (int c = 0; c < _n; ++c) {
                if (isSet(r, c)) {
                    if (canMoveL(r, c)) {
                        res.emplace_back(r, c, Move::L);
                    }
                    if (canMoveR(r, c)) {
                        res.emplace_back(r, c, Move::R);
                    }
                    if (canMoveU(r, c)) {
                        res.emplace_back(r, c, Move::U);
                    }
                    if (canMoveD(r, c)) {
                        res.emplace_back(r, c, Move::D);
                    }
                }
            }
        }
        return res;
    }
};

std::ostream &operator<<(std::ostream &os, const Board &b) {
    for (int r = 0; r < b._n; ++r) {
        for (int c = 0; c < b._n; ++c) {
            cout << b.at(r, c);
        }
        cout << endl;
    }
    return os;
}

bool solve_naive(Board b) {
    if (b.solved()) {
        return true;
    }
    vector<Move> moves = b.moves();
    if (moves.empty()) {
        return false;
    }
    for (const auto &m: moves) {
        b.move(m);
        // cout << b << endl;
        if (solve_naive(b)) {
            return true;
        }
        b.rewind();
        // cout << b << endl;
    }
    return false;
}

int main() {
    const int size = 7;
    char proto[size][size] = {
            {'x', 'x', '1', '1', '1', 'x', 'x'},
            {'x', 'x', '1', '1', '1', 'x', 'x'},
            {'1', '1', '1', '1', '1', '1', '1'},
            {'1', '1', '1', '0', '1', '1', '1'},
            {'1', '1', '1', '1', '1', '1', '1'},
            {'x', 'x', '1', '1', '1', 'x', 'x'},
            {'x', 'x', '1', '1', '1', 'x', 'x'}
    };
    Board b(size, pair<short, short>(3, 3));
    for (int r = 0; r < size; ++r) {
        for (int c = 0; c < size; ++c) {
            b.set(r, c, proto[r][c]);
        }
    }

    if (solve_naive(b)) {
        cout << "solved" << endl;
    } else {
        cout << "cannot solve" << endl;
    }
}