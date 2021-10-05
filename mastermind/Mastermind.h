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
#include <sstream>

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

    [[nodiscard]] bool sameAs(const Pigs &o) const {
        return w == o.w && b == o.b;
    }

    [[nodiscard]] bool allBlacks() const {
        return b == PIN_SIZE;
    }
};

std::ostream &operator<<(std::ostream &os, const Pigs &a) {
    return os << "b: " << a.b << " w: " << a.w;
}

//-----------------------------------------------------------------------
class State {
public:
    array<char, 4> v{};

    State() : v({'0', '0', '0', '0'}) {}

    explicit State(size_t value) {
        std::to_chars(v.begin(), v.end(), value);
    }

    char operator[](int i) const { return v[i]; }

    char &operator[](int i) { return v[i]; }

    friend bool operator==(const State &s1, const State &s2) {
        return s1.v == s2.v;
    }
};

std::ostream &operator<<(std::ostream &os, const State &g) {
    for (char c: g.v) {
        os << std::to_string(c - '0');
    }
    return os;
}

std::string to_string(const State &s) {
    ostringstream ss;
    ss << s;
    return ss.str();
}

//-----------------------------------------------------------------------
class Mastermind {
private:
    const static int CLS = 5; // number of colors
    const static int PPR = 4; // pins per row

    const int _size = (int) pow(6, PPR);
    vector<State> _hist;
    vector<State> _sspace;

    void setup();

    State nextGuess(const Pigs &a);

public:
    Mastermind() : _sspace(_size) {
        setup();
    }

    /**
     * Computes an initial guess.
     *
     * @return the guess
     */
    State solve();

    /**
     * Computes a new guess based on the result of the last one.
     *
     * @param a the pigs of the last guess
     * @return the new guess
     */
    State solve(const Pigs &a);

    /**
     * Obtains the amount of pigs by comparing two guesses.
     *
     * @param a the first guess
     * @param b the second guess
     * @return the amount of pigs
     */
    static Pigs answer(State a, State b);
};

//-----------------------------------------------------------------------
bool playAuto(Mastermind &mm, State &sol) {
    State r;
    Pigs a{};
    int maxTries = 10;

    int i = 0;
    while (!a.allBlacks() && i++ < maxTries) {
        r = mm.solve(a);
        a = Mastermind::answer(sol, r);
    }
    return i < maxTries;
}

int playAuto() {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(1, 5);

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    int tries = 100000;
    duration<double, std::milli> sum{};
    for (int i = 0; i < tries; ++i) {
        Mastermind mm;
        State sol;
        for (int j = 0; j < 4; ++j) {
            sol[j] = (char) (dist(rng) + '0');
        }

        auto t1 = high_resolution_clock::now();
        if (!playAuto(mm, sol)) {
            throw logic_error("not solved " + to_string(sol));
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

    State r;
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
    //return playManual();
}

#endif //HSNR_WSY_MASTERMIND_H
