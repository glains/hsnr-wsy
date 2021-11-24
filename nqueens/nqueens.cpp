
#include <array>
#include <iostream>
#include <chrono>
#include <exception>

using namespace std;

bool solvable(const int *cols, int row) {
    for (int i = 0; i < row; ++i) {
        if (cols[row] == cols[i]) {
            return false;
        }
        int dr = abs(row - i);
        int dc = abs(cols[row] - cols[i]);
        if (dr == dc) {
            return false;
        }
    }
    return true;
}

bool solve(int n, int *cols, int row) {
    if (row >= n) {
        return true;
    }
    for (int i = 0; i < n; ++i) {
        cols[row] = i;
        if (solvable(cols, row)) {
            if (solve(n, cols, row + 1)) {
                return true;
            }
        }
    }
    return false;
}

bool solve(int n) {
    int *cols = new int[n];
    return solve(n, cols, 0);
}

//-----------------------------------------------------------------------

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

#define DUR_T duration<long long int, std::nano>

void benchmark(int n) {
    const int tries = 1;

    DUR_T elapsed = high_resolution_clock::duration::zero();
    for (int i = 0; i < tries; ++i) {

        auto t1 = high_resolution_clock::now();
        if (!solve(n)) {
            throw logic_error("not solved");
        }
        auto t2 = high_resolution_clock::now();

        auto ms_int = duration_cast<milliseconds>(t2 - t1);
        elapsed += t2 - t1;
    }
    DUR_T total = elapsed / tries;
    duration<double, std::milli> total_ms = total;
    double ms = total_ms.count();

    std::cout << "avg/x -- time: " << ms << " ms." << std::endl;
}

//-----------------------------------------------------------------------

int main() {
    benchmark(31);
    return 0;
}