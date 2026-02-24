#include "collider.hpp"

SquareCollider::SquareCollider(Vector2D<int> *position, int width, int height)
    : position(position), width(width), height(height) {};

SquareCollider& SquareCollider::operator=(const SquareCollider& other) {
    if (this == &other) {
        return *this;
    }
    position = other.position;
    width = other.getWidth();
    height = other.getHeight();
    return *this;
}

bool SquareCollider::collidesWith(const SquareCollider& other) const {
    return (position->getX() < other.position->getX() + other.getWidth() &&
            position->getX() + width > other.position->getX() &&
            position->getY() < other.position->getY() + other.getHeight() &&
            position->getY() + height > other.position->getY());
}

int SquareCollider::getX() const {
    return position->getX();
}

int SquareCollider::getY() const {
    return position->getY();
}

int SquareCollider::getWidth() const {
    return width;
}

int SquareCollider::getHeight() const {
    return height;
}
