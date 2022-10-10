#pragma once
#include <cctype>

struct Position {
    int x;
    int y;

    Position& operator+=(const Position& other) noexcept;
    Position operator+(const Position& other) const noexcept;
    Position& operator-=(const Position& other) noexcept;
    Position operator-(const Position& other) const noexcept;
    Position operator-() const noexcept;

    void rotate(Position center, bool clockwise) noexcept;
    void rotate_bpx(Position center, bool clockwise) noexcept;
};
