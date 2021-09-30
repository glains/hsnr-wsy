#include "Mastermind.h"

#include <cmath>

void Mastermind::setup() {
    int min = 0;
    int max = 0;
    for (int i = 0; i < PPR; ++i) {
        min += 1 * (int) pow(10, i);
        max += CLS * (int) pow(10, i);
    }

    array<char, PPR> arr{'0', '0', '0', '0'};
    _space.push_back(arr);
    int i = 0;
    while (i < _size) {
        int d = PPR - 1;
        arr[d]++;
        while (arr[d] > CLS + '0') {
            arr[d--] = '1';
            arr[d]++;
        }
        _space[i] = (arr);
        ++i;
    }
}

array<char, 4> Mastermind::solve() {
    array<char, 4> guess{'1', '1', '2', '2'};
    _hist.push_back(guess);
    return guess;
}

array<char, 4> Mastermind::solve(const Pigs &a) {
    if (a.b == 4) {
        return _hist.back(); // solved
    }
    if (_hist.empty()) {
        return solve();
    }
    array<char, 4> guess = nextGuess(a);
    _hist.push_back(guess);
    return guess;
}

array<char, 4> Mastermind::nextGuess(const Pigs &a) {
    array<char, 4> last = _hist.back();

    size_t size = _space.size();
    vector<int> invalid;
    for (int i = 0; i < _space.size(); ++i) {
        const Pigs &sans = answer(last, _space[i]);
        if (!sans.sameAs(a)) {
            invalid.push_back(i);
        }
    }

    for (size_t i = invalid.size(); i-- > 0;) {
        _space.erase(_space.begin() + invalid[i]);
    }

    return *_space.begin();
}

Pigs Mastermind::answer(array<char, 4> a, array<char, 4> b) {
    short black = 0;
    for (int i = 0; i < PPR; ++i) {
        char gd = a[i];
        char cd = b[i];
        if (gd == cd) {
            black++;
            a[i] = '9';
            b[i] = '9';
        }
    }

    short white = 0;
    for (int i = 0; i < PPR; ++i) {
        char gd = a[i];
        for (int j = 0; j < PPR; ++j) {
            char cd = b[j];
            if (cd < '9' && gd == cd) {
                white++;
                a[i] = '9';
                b[j] = '9';
                break;
            }
        }
    }

    return {black, white};
}