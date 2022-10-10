#include "Shape.h"
#include "Map.h"
#include <random>
#include <cstring>

bool Shape::try_shift(const Map& map, Position delta) noexcept {
    shift(delta);

    if (in_range(map) && !overlap(map)) {
        return true;
    }
    else {
        shift(-delta);
        return false;
    }
}

void Shape::shift(Position delta) noexcept {
    for (auto &i : positions)
        i += delta;
    center += delta;
}

void Shape::add_to_map(Map& map) const {
    for (auto i : positions)
        if (i.y >= 0)
            map.set(i, true);
}

void Shape::remove_from_map(Map &map) const {
    for (auto i : positions)
        if (i.y >= 0)
            map.set(i, false);
}

const Position &Shape::operator[](int idx) const noexcept {
    return positions[idx];
}

Shape::Type Shape::random_shape_type() noexcept {
    static std::random_device rd;
    static std::mt19937_64 mt(rd());
    static std::uniform_int_distribution<int> dist(0, 6);

    return Type(dist(mt));
}

bool Shape::in_range(const Map &map) const noexcept {
    for (auto p : positions) {
        if (p.x >= map.cols() || p.x < 0
            || p.y >= int(map.rows()))
            return false;
    }
    return true;
}

bool Shape::overlap(const Map &map) const {
    for (auto p : positions)
        if (p.y >= 0 && !map.is_empty(p))
            return true;
    return false;
}

bool Shape::try_rotate(const Map &map, bool clockwise) noexcept {
    if (type == Type::O)
        return true;

    rotate(clockwise);

    if (in_range(map) && !overlap(map)) {
        return true;
    }
    else {
        rotate(!clockwise);
        return false;
    }
}

void Shape::rotate(bool clockwise) noexcept {
    if (type == Type::I) {
        for (auto& pos : positions)
            pos.rotate_bpx(center, flip_flop);
        flip_flop = !flip_flop;
    }
    else {
        if (type == Type::Z || type == Type::S) {
            for (auto& pos : positions)
                pos.rotate(center, flip_flop);
            flip_flop = !flip_flop;
        }
        else {
            for (auto &pos: positions)
                pos.rotate(center, clockwise);
        }
    }
}

Shape::Shape(Position base)
    : Shape(base, random_shape_type()) {
}


Shape::Shape(Position base, Type type) {
    // First element is central element, except for O and I
    static constexpr Position shape[][shape_size] = {
        { {0, 1}, {-1, 1}, {1, 1}, {2, 1} },    // i
        { {0, 0}, {1, 1}, {1, 0}, {0, 1} },     // o
        { {0, 0}, {-1, 0}, {1, 0}, {1, 1} },    // j
        { {0, 0}, {-1, 0}, {1, 0}, {-1, 1} },   // l
        { {0, 0}, {-1, 1}, {0, 1}, {1, 0} },    // s
        { {0, 0}, {-1, 0}, {0, 1}, {1, 1} },    // z
        { {0, 0}, {-1, 0}, {1, 0}, {0, 1} }     // t
    };

    this->type = type;
    center = base;
    memcpy(positions.data(), shape[int(type)], sizeof(Position) * shape_size);
    for (auto& p : positions)
        p += base;
    flip_flop = false;
}

bool Shape::all_out() const noexcept {
    for (auto p : positions)
        if (p.y >= 0)
            return false;
    return true;
}
