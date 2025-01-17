#pragma once

#include "Vec2.hpp"
#include <string>
#include <SFML/Graphics/Color.hpp>

// Transform component: stores position, velocity, and scale
struct CTransform {
    Vec2<float> position;   // Entity's position
    Vec2<float> velocity;   // Entity's velocity
    Vec2<float> scale = {1.0f, 1.0f}; // Scale (default is 1.0)

    CTransform(const Vec2<float>& pos = {0.0f, 0.0f},
               const Vec2<float>& vel = {0.0f, 0.0f},
               const Vec2<float>& scl = {1.0f, 1.0f})
        : position(pos), velocity(vel), scale(scl) {}
};

// Bounding Box component: defines a rectangle for collision detection
struct CBoundingBox {
    float width;  // Width of the bounding box
    float height; // Height of the bounding box

    CBoundingBox(float w = 0.0f, float h = 0.0f) : width(w), height(h) {}
};

struct CShape {
    int sides = 0;
    float radius = 0.0f;
    sf::Color color = sf::Color::White;

    CShape() = default; // Default constructor
    CShape(int s, float r, const sf::Color& c) : sides(s), radius(r), color(c) {}
};

struct CState {
    bool isInvincible = false; // True if the player is invincible
    float invincibilityTimer = 0.0f; // Remaining invincibility duration
    CState() = default; // Default constructor
    CState(bool i, float t) : isInvincible(i), invincibilityTimer(t) {}
};

struct CRotation {
    float angle = 0.0f;        // Current rotation angle in degrees
    float speed = 0.0f;        // Rotation speed (degrees per second)

    CRotation(float initAngle = 0.0f, float rotationSpeed = 0.0f)
        : angle(initAngle), speed(rotationSpeed) {}
};

struct CAI {
    std::string behavior = "random"; // Currently, only "random" behavior is supported

    CAI(const std::string& behaviorType = "random") : behavior(behaviorType) {}
};

// Time-based lifespan: how long an entity stays alive in seconds
struct CLifeSpan {
    float remainingTime; // Remaining time (in seconds)
    float totalTime;     // Total lifespan

    CLifeSpan(float life = 0) : remainingTime(life), totalTime(life) {}
};

struct CSpawnTime {
    float timeSinceSpawn;

    CSpawnTime(float spawnTime = 0.0f) : timeSinceSpawn(spawnTime) {}
};

// Count-based lives: for entities like players
struct CLives {
    int remaining;
    int total;       // Number of lives remaining

    CLives(int lives = 0) : remaining(lives), total(lives) {}
};

// Input component: stores movement flags for entities controlled by input
struct CInput {
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;

    CInput() = default;
};

// Animation component: manages animation state
struct CAnimation {
    std::string textureID; // ID for the texture
    int frame = 0;         // Current animation frame
    int frameCount = 0;    // Total number of frames
    int frameSpeed = 0;    // Frame speed (frames per second)

    CAnimation(const std::string& texID = "", int frm = 0, int count = 0, int speed = 0)
        : textureID(texID), frame(frm), frameCount(count), frameSpeed(speed) {}
};

// Gravity component: applies gravitational force
struct CGravity {
    Vec2<float> force = {0.0f, 0.0f}; // Gravity force

    CGravity(const Vec2<float>& grav = {0.0f, 9.8f}) : force(grav) {}
};

struct CCollision {
    float radius;           // Collision radius
    bool isCollidable;      // Whether the entity can collide with others
    bool isTrigger;         // If true, this will only detect collisions without affecting physics

   CCollision() = default; // Default constructor
   CCollision(float r, bool collidable = true, bool trigger = false)
        : radius(r), isCollidable(collidable), isTrigger(trigger) {}
};

struct CBullet {
    float speed;        // Speed of the bullet
    bool active;        // Whether the bullet is currently active
    float lifeTime;     // How long the bullet survives after being fired

    CBullet() = default; 
    CBullet(float s, float lt) : speed(s), active(true), lifeTime(lt) {}
};
