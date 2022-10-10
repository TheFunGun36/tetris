#include "Position.h"

Position Position::operator+(const Position &other) const noexcept {
    return {x + other.x, y + other.y};
}

Position& Position::operator+=(const Position &other) noexcept {
    x += other.x;
    y += other.y;
    return *this;
}

Position& Position::operator-=(const Position &other) noexcept {
    x -= other.x;
    y -= other.y;
    return *this;
}

Position Position::operator-(const Position &other) const noexcept {
    return {x - other.x, y - other.y};
}

Position Position::operator-() const noexcept {
    return {-x, -y};
}

void Position::rotate(Position center, bool clockwise) noexcept {
    *this -= center;

    int tmp = x;
    x = y;
    y = tmp;

    if (clockwise)
        x = -x;
    else
        y = -y;

    *this += center;
}

void Position::rotate_bpx(Position center, bool clockwise) noexcept {
    *this -= center;

    int tmp = 2 * x - 1;
    x = 2 * y - 1;
    y = tmp;

    if (clockwise)
        x = -x;
    else
        y = -y;

    x = (x + 1) / 2;
    y = (y + 1) / 2;

    *this += center;
}