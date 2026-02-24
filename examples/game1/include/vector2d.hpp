#pragma once

template<typename T>
class Vector2D {
public:
    Vector2D();
    Vector2D(T x, T y);

    T getX() const;
    T getY() const;
    void setX(T x);
    void setY(T y);

    void operator+=(const Vector2D<T>& other);
    void operator-=(const Vector2D<T>& other);
    void operator*=(T scalar);

private:
    T x;
    T y;
};

// dont ask

template<typename T>
Vector2D<T>::Vector2D() : x(0), y(0) {}

template<typename T>
Vector2D<T>::Vector2D(T x, T y) : x(x), y(y) {}

template<typename T>
T Vector2D<T>::getX() const {
    return x;
}

template<typename T>
T Vector2D<T>::getY() const {
    return y;
}

template<typename T>
void Vector2D<T>::setX(T x) {
    this->x = x;
}

template<typename T>
void Vector2D<T>::setY(T y) {
    this->y = y;
}

template<typename T>
void Vector2D<T>::operator+=(const Vector2D<T>& other) {
    x += other.getX();
    y += other.getY();
}

template<typename T>
void Vector2D<T>::operator-=(const Vector2D<T>& other) {
    x -= other.getX();
    y -= other.getY();
}

template<typename T>
void Vector2D<T>::operator*=(T scalar) {
    x *= scalar;
    y *= scalar;
}