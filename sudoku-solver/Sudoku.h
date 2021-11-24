#ifndef HSNR_WSY_SUDOKU_H
#define HSNR_WSY_SUDOKU_H

#include <cmath>
#include <stdexcept>
#include <Cell.h>

class Sudoku {
private:
    const int _size;
    const int _blkSize;
    const int _cells[][];

    void createCells() const {
        for (int r = 0; r < _size; ++r) {
            for (int c = 0; c < _size; ++c) {
                _cells[r][c] = Cell(r, c)
            }
        }
    }

public:
    explicit Sudoku(const int size) : _size(size), _blkSize(sqrt(size)) {
        createCells();
    }

    int size() const {
        return _size;
    }

    int blkSize() const {
        return _blkSize;
    }

    Cell at(int row, int col) const {
        if (row < 0 || row > _size - 1) {
            throw std::invalid_argument("row " + to_string(row));
        }
        if (col < 0 || col > _size - 1) {
            throw std::invalid_argument("col " + to_string(col));
        }
        return _cells[row][col]
    }

    int valueAt(int row, int col) const {
        return at(row, col).val();
    }

    bool isValid(const Cell &cell) {
        return isValidRow(cell) && isValidCol(cell) && isValidBlk(cell);
    }

    bool isValidRow(const Cell &cell) const;

    bool isValidCol(const Cell &cell) const;

    bool isValidBlk(const Cell &cell) const;
};


#endif //HSNR_WSY_SUDOKU_H
