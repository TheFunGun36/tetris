#include "Map.h"
#include <cstring>

Map::Map(size_t sz_x, size_t sz_y) {
    this->sz_x = sz_x;
    this->sz_y = sz_y;

    map = new Cell * [sz_y];
    for (int i = 0; i < sz_y; i++) {
        map[i] = new Cell[sz_x];
        memset(map[i], ' ', sz_x * sizeof(Cell));
    }
}

Map::~Map() {
    for (int i = 0; i < sz_y; i++)
        delete[] map[i];
    delete[] map;
}

Map::Cell Map::get(size_t x, size_t y) const {
    return map[y][x];
}

Map::Cell Map::get(const Position& p) const {
    return map[p.y][p.x];
}

void Map::set(size_t x, size_t y, bool value) {
    map[y][x] = value ? cell_set : cell_empty;
}

void Map::set(const Position& p, bool value) {
    map[p.y][p.x] = value ? cell_set : cell_empty;
}

const Map::Cell* Map::row(size_t row) const {
    return map[row];
}

const Map::Cell* Map::row_end(size_t row) const {
    return map[row] + sz_x;
}

bool Map::is_full(size_t row) const {
    const Cell* end = map[row] + sz_x;
    for (Cell* c = map[row]; c < end; c++)
        if (*c == cell_empty)
            return false;
    return true;
}

size_t Map::rows() const {
    return sz_y;
}

size_t Map::cols() const {
    return sz_x;
}

bool Map::is_empty(size_t x, size_t y) const {
    return get(x, y) == cell_empty;
}

bool Map::is_empty(const Position& p) const {
    return get(p) == cell_empty;
}

void Map::fill_row(size_t row, bool value) {
    memset(map[row], value ? cell_set : cell_empty, sz_x * sizeof(Cell));
}

void Map::move_row(size_t dst, size_t src) {
    memcpy(map[dst], map[src], sz_x * sizeof(Cell));
    fill_row(src, false);
}

bool Map::is_empty(size_t row) const {
    const Cell* end = map[row] + sz_x;
    for (Cell* c = map[row]; c < end; c++)
        if (*c != cell_empty)
            return false;
    return true;
}
