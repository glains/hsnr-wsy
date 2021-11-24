#include "vector"
#include "random"
#include <iostream>
#include "algorithm"

using namespace std;

void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

size_t partition(vector<int> &a, int l, int r) {
    random_device dev;
    mt19937 rng(dev());
    uniform_int_distribution<std::mt19937::result_type> dist(0, a.size() - 1);

    int p = a[l];
    //swap(a[l], a[p]);

    int i = l + 1;
    int j = r;

    do {
        while (i < r && a[i] <= p) i++;
        while (j > l && a[j] >= p) j--;
        if (i < j) swap(&a[i], &a[j]);
    } while (j > i);

    swap(&a[l], &a[j]);
    return j;
}

void qsort(vector<int> &a, size_t l, size_t r) {
    if (l < r) {
        size_t mid = partition(a, l, r);
        qsort(a, l, mid - 1);
        qsort(a, mid + 1, r);
    }
}

void qsort(vector<int> &a) {
    qsort(a, 0, a.size() - 1);
}

int main() {
    random_device dev;
    mt19937 rng(dev());
    uniform_int_distribution<std::mt19937::result_type> dist(0, 100);


    int min = 15;
    int max = 25;
    for (int i = 2; i < 6; ++i) {
        auto size = (size_t) pow(2, i);
        vector<int> a;
        a.reserve(size);
        for (int j = 0; j < size; ++j) {
            a.push_back((int) dist(rng));
        }

        qsort(a);

        for (int k = 0; k < size; ++k) {
            cout << a[k] << " ";
        }
        cout << endl;
    }

    return 0;
}