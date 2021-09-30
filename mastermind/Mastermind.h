#ifndef HSNR_WSY_MASTERMIND_H
#define HSNR_WSY_MASTERMIND_H

#include <set>
#include <vector>
#include <iostream>

#define TYPE short int
#define PIN_SIZE 4

using namespace std;

class Answer {
public:
    TYPE w, b;

    Answer() {}

    Answer(TYPE b, TYPE w) : b(b), w(w) {}

    bool sameAs(const Answer &o) const {
        return w == o.w && b == o.b;
    }

    bool done() const {
        return b == PIN_SIZE;
    }
};

std::ostream &operator<<(std::ostream &os, const Answer &a) {
    return os << "b: " << a.b << " w: " << a.w;
}

class Mastermind {
private:
    const TYPE CLS = 5; // number of colors
    const TYPE PPR = 4; // pins per row

    vector<TYPE> _hist;
    set<TYPE> _space;

    void setup();

public:
    Mastermind() {
        setup();
    }

    TYPE solve();

    TYPE solve(const Answer &a);

    TYPE nextGuess(const Answer &a);

    Answer answer(TYPE guess, TYPE cand) const;
};

int main() {
    Mastermind mm;
    TYPE sol = 5555;

    TYPE r;
    Answer a;

    int i = 0;
    while (!a.done() && i < 10) {
        r = mm.solve(a);
        std::cout << "guess:  " << r << endl;
        a = mm.answer(sol, r);
        std::cout << "answer: " << a << endl;
        i++;
    }
    std::cout << "tries needed " << i;
}

#endif //HSNR_WSY_MASTERMIND_H
