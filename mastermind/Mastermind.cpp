#include "Mastermind.h"

#include <cmath>

void Mastermind::setup() {
    State arr;
    _space.push_back(arr);
    int i = 0;
    while (i < _size) {
        int d = PPR - 1;
        arr[d]++;
        while (arr[d] > CLS + '0') {
            arr[d--] = '0';
            arr[d]++;
        }
        _space[i] = (arr);
        ++i;
    }
}

State Mastermind::solve() {
    State guess(1122);
    _hist.push_back(guess);
    return guess;
}

State Mastermind::solve(const Pigs &a) {
    if (a.b == 4) {
        return _hist.back(); // solved
    }
    if (_hist.empty()) {
        return solve();
    }
    State guess = nextGuess(a);
    _hist.push_back(guess);
    return guess;
}

State Mastermind::nextGuess(const Pigs &a) {
    State last = _hist.back();

    vector<int> invalid;
    for (int i = 0; i < _space.size(); ++i) {
        const Pigs &pigs = answer(last, _space[i]);
        if (!pigs.sameAs(a)) {
            invalid.push_back(i);
        }
    }

    for (size_t i = invalid.size(); i-- > 0;) {
        _space.erase(_space.begin() + invalid[i]);
    }

    return *_space.begin();
}

Pigs Mastermind::answer(State a, State b) {
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