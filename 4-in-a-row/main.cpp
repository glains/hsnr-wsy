#include "game.h"

#include <chrono>
#include <iostream>

using namespace std;


using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

#define DUR_T duration<long long int, std::nano>

void test() {
    DUR_T elapsed = high_resolution_clock::duration::zero();

    ull l1 = 0x07;
    ull l2 = 0b100110;
    Board b2(l2, 0x0);

    auto t1 = high_resolution_clock::now();
    for (int t = 0; t < 1000000000; ++t) {
        b2.testScore();
    }
    auto t2 = high_resolution_clock::now();

    auto ms_int = duration_cast<milliseconds>(t2 - t1);
    elapsed += t2 - t1;

    duration<double, std::milli> total_ms = elapsed;
    double ms = total_ms.count();
    cout << "took " << ms << " ms" << endl;
}

void testCol() {
    ull l1 = 0b011100;
    ull l2 = 0b100010;
    Board b2(l1, l2);
    cout << "col: " << b2.testScore() << endl;
}

void testRow() {
    ull l1 = 0b100000010000000000000;
    ull l2 = 0b01001111;
    Board b2(l1, l2);
    cout << b2 << endl;
}

void testMove() {
    ull l1 = 0b0111000000;
    ull l2 = 0b0000000001;
    Board b2(l1, l2);
    auto move = b2.search(10);
    cout << b2 << endl;
    cout << b2.move(move.col) << endl;
}

void testMove2() {
    ull l1 = 0b0000000000;
    ull l2 = 0b0000000000;
    Board b2(l1, l2);
    //auto move = b2.search(10);
    cout << b2.move(0).move(1).move(2).move(3).move(4).move(5) << endl;
}

int getNextMove() {
    string line;
    getline(cin, line);
    return stoi(line);
}

void playSimple() {
    bool ownTurn = true;
    Board b;
    while (!b.won()) {
        if (ownTurn) {
            int col = getNextMove();
            b = b.move(col);
        } else {
            auto move = b.search(12);
            b = b.move(move.col);
        }

        cout << b << endl;

        ownTurn = !ownTurn;
    }
}

int main() {
    //testCol();
    //testRow();
    //testMove2();
    //test();
    playSimple();
}