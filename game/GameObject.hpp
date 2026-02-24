#pragma once

#include <cstdint>
#include "Vector.hpp"
#include "BoxCollider.hpp"
#include "displaylib_16/ili9341.hpp"

class GameObject {
protected:
    Vector2 position;
    Vector2 velocity;
    Vector2 scale;
    BoxCollider collider;
    bool active;
    bool visible;
    uint16_t color;
    uint32_t id;

    static uint32_t nextId;

public:
    GameObject(const Vector2& pos = Vector2(0.0f, 0.0f),
               const BoxCollider& col = BoxCollider(10.0f, 10.0f),
               uint16_t col_color = 0xFFFF)
        : position(pos), velocity(0.0f, 0.0f), scale(1.0f, 1.0f), collider(col),
          active(true), visible(true), color(col_color), id(nextId++) {}

    virtual ~GameObject() = default;

        enum class Type {
            Generic,
            Player,
            Asteroid
        };

        virtual Type getType() const { return Type::Generic; }

    // Update method - called every frame, user should override
    virtual void update(float deltaTime) {
        position = position + (velocity * deltaTime);
    }

    // Render method - called every frame, user should override
    virtual void render(ILI9341_TFT& display) = 0;

    // Getters
    Vector2 getPosition() const { return position; }
    Vector2 getVelocity() const { return velocity; }
    Vector2 getScale() const { return scale; }
    BoxCollider& getCollider() { return collider; }
    const BoxCollider& getCollider() const { return collider; }
    bool isActive() const { return active; }
    bool isVisible() const { return visible; }
    uint16_t getColor() const { return color; }
    uint32_t getId() const { return id; }

    // Setters
    void setPosition(const Vector2& pos) { position = pos; }
    void setVelocity(const Vector2& vel) { velocity = vel; }
    void setScale(const Vector2& s) { scale = s; }
    void setActive(bool a) { active = a; }
    void setVisible(bool v) { visible = v; }
    void setColor(uint16_t c) { color = c; }

    // Physics helpers
    void addForce(const Vector2& force) {
        velocity = velocity + force;
    }

    void addVelocity(const Vector2& vel) {
        velocity = velocity + vel;
    }

    void translate(const Vector2& offset) {
        position = position + offset;
    }

    // Collision detection
    bool collidesWith(GameObject& other) {
        if (!active || !other.active) return false;
        return collider.intersects(position, other.collider, other.position);
    }

    // Get distance to another game object
    float distanceTo(const GameObject& other) const {
        return position.distance(other.position);
    }

    // Get direction towards another game object
    Vector2 directionTo(const GameObject& other) const {
        return (other.position - position).normalized();
    }
};

