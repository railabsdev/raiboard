#pragma once

#include "Vector.hpp"

class BoxCollider {
private:
    float width, height;
    Vector2 offset;

public:
    BoxCollider(float w = 0.0f, float h = 0.0f, const Vector2& off = Vector2(0.0f, 0.0f))
        : width(w), height(h), offset(off) {}

    // Get width and height
    float getWidth() const { return width; }
    float getHeight() const { return height; }
    void setWidth(float w) { width = w; }
    void setHeight(float h) { height = h; }

    // Get and set offset
    Vector2 getOffset() const { return offset; }
    void setOffset(const Vector2& off) { offset = off; }

    // Get the center point of the collider in world space
    Vector2 getCenter(const Vector2& objectPos) const {
        return Vector2(
            objectPos.x + offset.x + width / 2.0f,
            objectPos.y + offset.y + height / 2.0f
        );
    }

    // Get the bounds (min and max corners)
    void getBounds(const Vector2& objectPos, Vector2& outMin, Vector2& outMax) const {
        outMin = Vector2(objectPos.x + offset.x, objectPos.y + offset.y);
        outMax = Vector2(outMin.x + width, outMin.y + height);
    }

    // Check if two axis-aligned bounding boxes intersect
    bool intersects(const Vector2& pos1, const BoxCollider& other, const Vector2& pos2) const {
        Vector2 min1, max1, min2, max2;
        getBounds(pos1, min1, max1);
        other.getBounds(pos2, min2, max2);

        return min1.x < max2.x &&
               max1.x > min2.x &&
               min1.y < max2.y &&
               max1.y > min2.y;
    }

    // Check if a point is inside the collider
    bool containsPoint(const Vector2& pos, const Vector2& point) const {
        Vector2 min, max;
        getBounds(pos, min, max);
        return point.x >= min.x && point.x <= max.x &&
               point.y >= min.y && point.y <= max.y;
    }

    // Get the overlap amount between two colliders (useful for collision response)
    Vector2 getOverlap(const Vector2& pos1, const BoxCollider& other, const Vector2& pos2) const {
        Vector2 min1, max1, min2, max2;
        getBounds(pos1, min1, max1);
        other.getBounds(pos2, min2, max2);

        float overlapX = 0.0f;
        float overlapY = 0.0f;

        if (min1.x < max2.x && max1.x > min2.x) {
            float leftOverlap = max2.x - min1.x;
            float rightOverlap = max1.x - min2.x;
            overlapX = (leftOverlap < rightOverlap) ? -leftOverlap : rightOverlap;
        }

        if (min1.y < max2.y && max1.y > min2.y) {
            float topOverlap = max2.y - min1.y;
            float bottomOverlap = max1.y - min2.y;
            overlapY = (topOverlap < bottomOverlap) ? -topOverlap : bottomOverlap;
        }

        return Vector2(overlapX, overlapY);
    }
};
