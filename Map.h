#pragma once
#include "Shape.h"
#include "Position.h"

class Map {
public:
    using Cell = char;
    static constexpr Cell cell_empty = ' ';
    static constexpr Cell cell_set = 219;

    Map(size_t sz_x, size_t sz_y);
    ~Map();

    bool is_empty(size_t x, size_t y) const;
    bool is_empty(const Position& p) const;
    Cell get(size_t x, size_t y) const;
    Cell get(const Position& p) const;
    void set(size_t x, size_t y, bool value);
    void set(const Position& p, bool value);
    const Cell* row(size_t row) const;
    const Cell* row_end(size_t row) const;
    void fill_row(size_t row, bool value);
    void move_row(size_t dst, size_t src);

    bool is_full(size_t row) const;
    bool is_empty(size_t row) const;

    size_t rows() const;
    size_t cols() const;

private:
    Cell** map;
    size_t sz_x;
    size_t sz_y;
};

