#pragma once

#include <algorithm>
#include <cmath>

struct Vector2D {

    constexpr int operator[](int i) const noexcept
    {
        return ((int*)this)[i];
    }

    constexpr int& operator[](int i) noexcept
    {
        return ((int*)this)[i];
    }

    constexpr auto operator==(const Vector2D& v) const noexcept
    {
        return x == v.x && y == v.y;
    }

    constexpr auto operator!=(const Vector2D& v) const noexcept
    {
        return !(*this == v);
    }

    constexpr Vector2D& operator=(const int array[3]) noexcept
    {
        x = array[0];
        y = array[1];
        return *this;
    }

    constexpr Vector2D& operator+=(const Vector2D& v) noexcept
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    constexpr Vector2D& operator+=(int f) noexcept
    {
        x += f;
        y += f;
        return *this;
    }

    constexpr Vector2D& operator-=(const Vector2D& v) noexcept
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    constexpr Vector2D& operator-=(int f) noexcept
    {
        x -= f;
        y -= f;
        return *this;
    }

    constexpr auto operator-(const Vector2D& v) const noexcept
    {
        return Vector2D{ x - v.x, y - v.y};
    }

    constexpr auto operator+(const Vector2D& v) const noexcept
    {
        return Vector2D{ x + v.x, y + v.y};
    }

    constexpr auto operator*(const Vector2D& v) const noexcept
    {
        return Vector2D{ x * v.x, y * v.y};
    }

    constexpr auto operator*=(int mul) noexcept
    {
        x *= mul;
        y *= mul;
        return *this;
    }

    constexpr Vector2D& operator/=(int div) noexcept
    {
        x /= div;
        y /= div;
        return *this;
    }

    constexpr auto operator/(int div) const noexcept
    {
        return Vector2D{ x / div, y / div};
    }

    constexpr auto operator*(int mul) const noexcept
    {
        return Vector2D{ x * mul, y * mul };
    }

    constexpr auto operator-(int sub) const noexcept
    {
        return Vector2D{ x - sub, y - sub };
    }

    constexpr auto operator+(int add) const noexcept
    {
        return Vector2D{ x + add, y + add };
    }

    int x, y;
};