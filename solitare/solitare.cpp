
#include <array>
#include <vector>
#include <stack>
#include <iostream>
#include <chrono>
#include <algorithm>

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
    vector<short *> _ptables;
    const pair<short, short> _lastPin;
    const int _lastPinIdx;
    int _pins = 0;
    stack<Move> _hist;

    explicit Board(int n, const pair<short, short> lastPin) :
            _n(n), _size(n * n), _lastPin(lastPin),
            _lastPinIdx(idx(lastPin.first, lastPin.second)) {
        _arr = new char[_size];
    }

    Board(const Board &o) :
            _n(o._n), _size(o._size), _lastPin(o._lastPin), _lastPinIdx(o._lastPinIdx) {
        _arr = new char[_size];
        std::copy(o._arr, o._arr + _size, _arr);
        _pins = o._pins;
    }

    [[nodiscard]] bool solved() const {
        return _pins == 1 && isSet(_lastPin.first, _lastPin.second);
    }

    void set(int row, int col, char val) {
        _arr[row * _n + col] = val;
        if (val == SET) _pins++;
    }

    [[nodiscard]] inline int idx(int row, int col) const {
        return row * _n + col;
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

    [[nodiscard]] bool solvable() const {
        for (const auto &t: _ptables) {
            int total = t[idx(_lastPin.first, _lastPin.second)];
            for (int i = 0; i < _size; ++i) {
                if (_arr[i] == SET) {
                    total += t[i];
                }
            }
            if (total < 0) {
                return false;
            }
        }
        return true;
    }

    void addPagoda(short *arr) {
        _ptables.push_back(arr);
    }

    [[nodiscard]] vector<Move> history() const {
        size_t n = _hist.size();
        stack<Move> hist(_hist);

        vector<Move> res;
        for (size_t i = 0; i < n; ++i) {
            res.push_back(hist.top());
            hist.pop();
        }
        reverse(res.begin(), res.end());
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

//-----------------------------------------------------------------------

bool solve(Board &b) {
    if (b.solved()) {
        return true;
    }
    if (!b.solvable()) {
        return false;
    }
    vector<Move> moves = b.moves();
    if (moves.empty()) {
        return false;
    }
    for (const auto &m: moves) {
        b.move(m);
        if (solve(b)) {
            return true;
        }
        b.rewind();
    }
    return false;
}

void initPagoda77(Board &b) {
    const int size = 7;
    auto *pagoda1 = new short[size * size]{
            0, 0, -1, 0, -1, 0, 0,
            0, 0, 1, 1, 1, 0, 0,
            -1, 1, 0, 1, 0, 1, -1,
            0, 1, 1, 2, 1, 1, 0,
            -1, 1, 0, 1, 0, 1, -1,
            0, 0, 1, 1, 1, 0, 0,
            0, 0, -1, 0, -1, 0, 0
    };
    b.addPagoda(pagoda1);
}

//-----------------------------------------------------------------------

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

#define DUR_T duration<long long int, std::nano>

void benchmark(Board &b) {
    const int tries = 100;

    // warmup
    for (int i = 0; i < tries / 2; ++i) {
        Board tmp(b);
        if (!solve(tmp)) {
            throw logic_error("not solved");
        }
    }

    size_t moves = 0;
    DUR_T elapsed = high_resolution_clock::duration::zero();
    for (int i = 0; i < tries; ++i) {

        Board tmp(b);
        auto t1 = high_resolution_clock::now();
        if (!solve(tmp)) {
            throw logic_error("not solved");
        }
        auto t2 = high_resolution_clock::now();

        vector<Move> hist = tmp.history();
        moves += hist.size();

        auto ms_int = duration_cast<milliseconds>(t2 - t1);
        elapsed += t2 - t1;
    }
    DUR_T total = elapsed / tries;
    duration<double, std::milli> total_ms = total;
    double ms = total_ms.count();
    double avg_moves = (double) moves / tries;

    std::cout << "avg/x -- time: " << ms << " ms. ; moves: " << avg_moves << std::endl;
}

//-----------------------------------------------------------------------

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
    Board b2(b);
    initPagoda77(b2);

    cout << "solving naive" << endl;
    benchmark(b);
    cout << "solving pagoda" << endl;
    benchmark(b2);

    return 0;
}