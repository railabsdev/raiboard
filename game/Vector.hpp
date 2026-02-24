#pragma once

#include <cmath>

class Vector2 {
public:
    float x, y;

    Vector2(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}

    // Vector addition
    Vector2 operator+(const Vector2& other) const {
        return Vector2(x + other.x, y + other.y);
    }

    // Vector subtraction
    Vector2 operator-(const Vector2& other) const {
        return Vector2(x - other.x, y - other.y);
    }

    // Scalar multiplication
    Vector2 operator*(float scalar) const {
        return Vector2(x * scalar, y * scalar);
    }

    // Scalar division
    Vector2 operator/(float scalar) const {
        if (scalar != 0.0f) {
            return Vector2(x / scalar, y / scalar);
        }
        return Vector2(0.0f, 0.0f);
    }

    // Dot product
    float dot(const Vector2& other) const {
        return x * other.x + y * other.y;
    }

    // Magnitude (length)
    float magnitude() const {
        return std::sqrt(x * x + y * y);
    }

    // Square magnitude (faster, no sqrt)
    float sqrMagnitude() const {
        return x * x + y * y;
    }

    // Normalized vector (unit vector)
    Vector2 normalized() const {
        float mag = magnitude();
        if (mag > 0.0f) {
            return Vector2(x / mag, y / mag);
        }
        return Vector2(0.0f, 0.0f);
    }

    // Distance between two points
    float distance(const Vector2& other) const {
        return (*this - other).magnitude();
    }

    // Lerp (linear interpolation) between two vectors
    static Vector2 lerp(const Vector2& a, const Vector2& b, float t) {
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;
        return Vector2(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t);
    }

    // Zero vector
    static Vector2 zero() {
        return Vector2(0.0f, 0.0f);
    }

    // Unit vectors
    static Vector2 right() {
        return Vector2(1.0f, 0.0f);
    }

    static Vector2 up() {
        return Vector2(0.0f, 1.0f);
    }

    static Vector2 one() {
        return Vector2(1.0f, 1.0f);
    }
};
