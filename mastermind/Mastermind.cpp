#include "Mastermind.h"

#include <cmath>

void Mastermind::setup() {
    TYPE min = 0;
    TYPE max = 0;
    for (int i = 0; i < PPR; ++i) {
        min += 1 * pow(10, i);
        max += CLS * pow(10, i);
    }
    for (TYPE i = min; i <= max; i++) {
        _space.insert(i);
    }
}

TYPE Mastermind::solve() {
    TYPE guess = 1122;
    _hist.push_back(guess);
    return guess;
}

TYPE Mastermind::solve(const Answer &a) {
    if (a.b == 4) {
        return _hist.back(); // solved
    }
    if (_hist.empty()) {
        return solve();
    }
    TYPE guess = nextGuess(a);
    _hist.push_back(guess);
    return guess;
}

TYPE Mastermind::nextGuess(const Answer &a) {
    TYPE last = _hist.back();

    vector<TYPE> invalid;
    for (const auto &s: _space) {
        const Answer &sans = answer(last, s);
        if (!sans.sameAs(a)) {
            invalid.push_back(s);
        }
    }
    for (const auto &val: invalid) {
        _space.erase(val);
    }

    return *_space.begin();
}

Answer Mastermind::answer(TYPE guess, TYPE cand) const {
    TYPE b = 0;
    for (int i = 0; i < PPR; ++i) {
        int p = (int) pow(10, i);
        int gd = (guess / p) % 10;
        int cd = (cand / p) % 10;
        if (cd < 9 && gd == cd) {
            b++;
            guess += (9 - gd) * p;
            cand += (9 - cd) * p;
        }
    }

    TYPE w = 0;
    for (int i = 0; i < PPR; ++i) {
        int gp = (int) pow(10, i);
        int gd = (guess / gp) % 10;
        for (int j = 0; j < PPR; ++j) {
            int cp = (int) pow(10, j);
            int cd = (cand / cp) % 10;
            if (cd < 9 && gd == cd) {
                w++;
                guess += (9 - gd) * gp;
                cand += (9 - cd) * cp;
                break;
            }
        }
    }

    return {b, w};
}

