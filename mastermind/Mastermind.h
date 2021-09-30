#ifndef HSNR_WSY_MASTERMIND_H
#define HSNR_WSY_MASTERMIND_H

#include <set>
#include <vector>
#include <array>
#include <iostream>
#include <valarray>
#include <charconv>

#define PIN_SIZE 4

using namespace std;

//-----------------------------------------------------------------------
class Pigs {
public:
    short b, w;

    Pigs() : b(0), w(0) {}

    Pigs(short b, short w) : b(b), w(w) {}

    bool sameAs(const Pigs &o) const {
        return w == o.w && b == o.b;
    }

    bool allBlacks() const {
        return b == PIN_SIZE;
    }
};

std::ostream &operator<<(std::ostream &os, const Pigs &a) {
    return os << "b: " << a.b << " w: " << a.w;
}

//-----------------------------------------------------------------------
class Guess {
private:
    array<char, 4> v;
public:
    Guess() : v({'0', '0', '0', '0'}) {}

    explicit Guess(size_t value) {
        std::to_chars(v.begin(), v.end(), value);
    }
};

//-----------------------------------------------------------------------
class Mastermind {
private:
    const static int CLS = 5; // number of colors
    const static int PPR = 4; // pins per row

    const int _size = (int) pow(6, PPR);
    vector<array<char, PPR>> _hist;
    vector<array<char, PPR>> _space;

    void setup();

public:
    Mastermind() : _space(_size) {
        setup();
    }

    array<char, 4> solve();

    array<char, 4> solve(const Pigs &a);

    array<char, 4> nextGuess(const Pigs &a);

    static Pigs answer(array<char, 4>, array<char, 4> b);

    static array<char, 4> from(size_t value);
};

//-----------------------------------------------------------------------
int main() {
    Mastermind mm;
    array<char, 4> sol = Mastermind::from(5555);

    array<char, 4> r{'0', '0', '0', '0'};
    Pigs a{};

    int i = 0;
    while (!a.allBlacks() && i < 10) {
        r = mm.solve(a);
        std::string str(r.begin(), r.end());
        std::cout << "guess:  " << str << endl;
        a = Mastermind::answer(sol, r);
        std::cout << "answer: " << a << endl;
        i++;
    }
    std::cout << "tries needed " << i;
}

#endif //HSNR_WSY_MASTERMIND_H
