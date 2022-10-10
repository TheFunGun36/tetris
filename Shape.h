#pragma once
#include "Position.h"
#include <initializer_list>
#include <array>

struct Map;

class Shape {
public:
    enum class Type { I, O, J, L, S, Z, T };

    explicit Shape(Position base);
    Shape(Position base, Type type);

    bool try_shift(const Map& map, Position delta) noexcept;
    void shift(Position delta) noexcept;

    bool try_rotate(const Map& map, bool clockwise) noexcept;
    void rotate(bool clockwise) noexcept;

    void add_to_map(Map& map) const;
    void remove_from_map(Map& map) const;

    bool all_out() const noexcept;

    const Position& operator[](int idx) const noexcept;
    static constexpr size_t shape_size = 4;

private:
    static Type random_shape_type() noexcept;

    bool in_range(const Map& map) const noexcept;
    bool overlap(const Map& map) const;

    Type type;
    std::array<Position, shape_size> positions;
    Position center;
    bool flip_flop;
};
