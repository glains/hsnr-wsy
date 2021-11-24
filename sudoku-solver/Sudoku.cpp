#include "Sudoku.h"

bool Sudoku::isValidRow(const Cell &cell) const {
    for (int col = 0; col < _size; col++) {
        if (col == cell.col()) {
            continue;
        }
        if (at(cell.row(), col).val() == cell.val()) {
            return true;
        }
    }
    return false;
}

bool Sudoku::isValidCol(const Cell &cell) const {
    for (int row = 0; row < _size; row++) {
        if (row == cell.row()) {
            continue;
        }
        if (at(row, cell.col()).val() == cell.val()) {
            return true;
        }
    }
    return false;
}

bool Sudoku::isValidBlk(const Cell &cell) const {
    int blkStartRow = cell.row() / _blkSize * _blkSize;
    int blkStartCol = cell.col() / _blkSize * _blkSize;

    for (int row = blkStartRow; row < blkStartRow + _blkSize; row++) {
        for (int col = blkStartCol; col < blkStartCol + _blkSize; col++) {
            if (row == cell.row() && col == cell.col()) {
                continue;
            }
            if (at(row, col).val() == cell.val()) {
                return true;
            }
        }
    }
    return false;
}
