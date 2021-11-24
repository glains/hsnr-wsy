#ifndef HSNR_WSY_CELL_H
#define HSNR_WSY_CELL_H

#include "vector"

using namespace std;

class Cell {
private:
    const int _row;
    const int _col;

    int _value;

    std::vector<int> candidates;

public:

    Cell(int row, int col) : _row(row), _col(col) {
        _value = 0;
        candidates = vector<int>();
    }

    Cell(int row, int col, int size) : _row(row), _col(col) {
        _value = 0;
        candidates = vector<int>(size);
    }

    int row() const {
        return _row;
    }

    int col() const {
        return _col;
    }

    int val() const {
        return _value;
    }

    const vector<int> &getCandidates() const {
        return candidates;
    }
};


#endif //HSNR_WSY_CELL_H
