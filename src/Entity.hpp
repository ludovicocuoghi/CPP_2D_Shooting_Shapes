#include <tuple>
#include <string>
#include "Components.hpp"

// Alias for the tuple that holds all possible components an entity can have
using ComponentTuple = std::tuple<
    CTransform, CLifeSpan, CLives, CInput, CBoundingBox, 
    CAnimation, CGravity, CShape, CAI, CRotation, CCollision, CState, CBullet, CSpawnTime
>;

// Entity class: Represents an object in the game world with components and metadata
class Entity {
private:
    ComponentTuple m_components;   // Stores all components for this entity
    bool m_alive = true;           // Tracks if the entity is active or destroyed
    std::string m_tag = "default"; // Entity type (e.g., "player", "enemy")
    size_t m_id = 0;               // Unique identifier for the entity

public:
    // === Constructor ===
    // Initializes an entity with a given tag and ID
    Entity(const std::string& tag, size_t id) 
        : m_tag(tag), m_id(id) {}

    // === Status Management ===
    // Checks if the entity is still active
    bool isAlive() const { return m_alive; }

    // Marks the entity as destroyed
    void destroy() { m_alive = false; }

    // === Metadata Access ===
    // Retrieves the entity's tag (descriptive label)
    const std::string& tag() const { return m_tag; }

    // Retrieves the entity's unique ID
    size_t id() const { return m_id; }

    // === Component Management ===
    // Adds a new component to the entity or replaces an existing one
    template <typename T, typename... Args>
    void add(Args&&... args) {
        // Constructs the component in-place with provided arguments
        std::get<T>(m_components) = T(std::forward<Args>(args)...);
    }

    // Retrieves a mutable reference to a specific component
    template <typename T>
    T& get() {
        return std::get<T>(m_components);
    }

    // Retrieves a constant reference to a specific component
    template <typename T>
    const T& get() const {
        return std::get<T>(m_components);
    }
};
