#pragma once

#include "vector2d.hpp"

class SquareCollider {
public:
    SquareCollider(Vector2D<int> *position, int width, int height);
    SquareCollider& operator=(const SquareCollider& other);
    SquareCollider(const SquareCollider& other);
    ~SquareCollider();

    bool collidesWith(const SquareCollider& other) const;
    int getX() const;
    int getY() const;
    int getWidth() const;
    int getHeight() const;
private:
    Vector2D<int> *position;
    int width;
    int height;
};