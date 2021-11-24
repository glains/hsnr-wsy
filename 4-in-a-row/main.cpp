#include "game.h"

#include <chrono>
#include <iostream>

using namespace std;


using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

#define DUR_T duration<long long int, std::nano>

int main() {

    ull l2 = 0b000001000001000001000001000001000001000001;
    Board b(l2, l2);

    //cout << b << endl;

    ull l1 = 0b001011000001000001000001000001000001000001;
    Board b2(l1, l1);

    ull l3 = 0b001111000001000001000001000001000001000001;
    Board b3(l3, l3);
    //cout << b3.won() << endl;

    ull l4 = 0b1000000100000010000001;
    ull l5 = 0b000000000000000000010000001000000100000010;
    ull m5 = 0b100000010000001000000100000000000000000000;
    ull l6 = 0b000000000000000000100000010000001000000100;

    Board b4(l4, l4);
    Board b5(l5, l5);
    Board b6(l6, l6);
    //cout << b4.won() << endl;
    //cout << b5.won() << endl;
    //cout << b6.won() << endl;

    DUR_T elapsed = high_resolution_clock::duration::zero();

    auto t1 = high_resolution_clock::now();
    for (int t = 0; t < 100000000; ++t) {
        b5.won();
    }
    auto t2 = high_resolution_clock::now();

    auto ms_int = duration_cast<milliseconds>(t2 - t1);
    elapsed += t2 - t1;

    duration<double, std::milli> total_ms = elapsed;
    double ms = total_ms.count();
    cout << "took " << ms << " ms" << endl;

    /*
    auto moves = b.nextMoves();
    for (const auto &m: moves) {
        cout << m << endl;
    }
    */

}