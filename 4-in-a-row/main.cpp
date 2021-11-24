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

int main() {
    ull l1 = 0b100100;
    ull l2 = 0b000011;
    Board b2(l1, l2);
    cout << b2.testScore() << endl;
    // test();
}