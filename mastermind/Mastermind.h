#ifndef HSNR_WSY_MASTERMIND_H
#define HSNR_WSY_MASTERMIND_H

#include <vector>
#include <array>
#include <string>
#include <iostream>
#include <valarray>
#include <charconv>
#include <random>
#include <chrono>

#define PIN_SIZE 4

using namespace std;

//-----------------------------------------------------------------------
class Pigs {
public:
    short b, w;

    Pigs() : b(0), w(0) {}

    Pigs(short b, short w) : b(b), w(w) {}

    explicit Pigs(const string &v) {
        if (v.size() > 2) {
            throw invalid_argument("expected 2 values");
        }
        b = (short) stoi(v.substr(0, 1));
        w = (short) stoi(v.substr(1, 2));
    }

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
public:
    array<char, 4> v{};

    Guess() : v({'0', '0', '0', '0'}) {}

    explicit Guess(size_t value) {
        std::to_chars(v.begin(), v.end(), value);
    }

    char operator[](int i) const { return v[i]; }

    char &operator[](int i) { return v[i]; }
};

std::ostream &operator<<(std::ostream &os, const Guess &g) {
    for (char c: g.v) {
        os << std::to_string(c - '0');
    }
    return os;
}

//-----------------------------------------------------------------------
class Mastermind {
private:
    const static int CLS = 5; // number of colors
    const static int PPR = 4; // pins per row

    const int _size = (int) pow(6, PPR);
    vector<Guess> _hist;
    vector<Guess> _space;

    void setup();

    Guess nextGuess(const Pigs &a);

public:
    Mastermind() : _space(_size) {
        setup();
    }

    Guess solve();

    Guess solve(const Pigs &a);

    static Pigs answer(Guess a, Guess b);
};

//-----------------------------------------------------------------------
bool playAuto(Mastermind &mm, Guess &sol) {
    Guess r;
    Pigs a{};
    int maxTries = 10;

    int i = 0;
    while (!a.allBlacks() && i < maxTries) {
        r = mm.solve(a);
        a = Mastermind::answer(sol, r);
        i++;
    }
    return i < maxTries;
}

int playAuto() {
    Mastermind mm;

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(1, 5);

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    int tries = 10000;
    duration<double, std::milli> sum{};
    for (int i = 0; i < tries; ++i) {
        Guess sol(5005);
        for (int j = 0; j < 4; ++j) {
            // sol[j] = (char) dist(rng);
        }

        auto t1 = high_resolution_clock::now();
        if (!playAuto(mm, sol)) {
            throw logic_error("not solved");
        }
        auto t2 = high_resolution_clock::now();

        auto ms_int = duration_cast<milliseconds>(t2 - t1);
        sum += t2 - t1;

    }
    std::cout << "solved " << tries << ", took " << sum.count() << "ms\n";

    return 0;
}

int playManual() {
    Mastermind mm;

    Guess r;
    Pigs a{};
    int maxTries = 10;

    int i = 0;
    while (!a.allBlacks() && i < maxTries) {
        r = mm.solve(a);
        cout << "guess     : " << r << endl;

        cout << "pigs(b,w) : ";
        string ans;
        getline(std::cin, ans);
        a = Pigs(ans);

        i++;
    }

    if (i < maxTries) {
        std::cout << "solved in " << i << " tries, solution is " << r << endl;
    } else {
        std::cout << "not solved in " << i << " tries" << endl;
    }

    return 0;
}

int main() {
    return playAuto();
    // return playManual();
}

#endif //HSNR_WSY_MASTERMIND_H
