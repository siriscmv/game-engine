#pragma once

#include <set>
#include <vector>
#include "Entity.h"

// A singleton class that handles collisions between game entities
class CollisionSystem {
public:
    static CollisionSystem& getInstance() {
        static CollisionSystem instance;                    // Singleton variable
        return instance;
    }

    CollisionSystem(const CollisionSystem&) = delete;         // Preventing copying
    void operator=(const CollisionSystem) = delete;         // Preventing assignmenet

    // Detect collisions between entities and apply physics, also returns a set of entities for which a collision was detected
    std::set<Entity*> run(const std::vector<Entity*>&);

    // Helper method to detect collision between 2 entities
    // NOTE: Currently handles only rectangle<->rectangle collisions
    bool hasCollision(const Entity* entityA, const Entity* entityB);

    bool hasCollisionRaw(const Entity* entityA, const Entity* entityB);

private:
    CollisionSystem() = default;
    ~CollisionSystem() = default;

    // Helper method to apply physics to 2 entities that are in collision
    void handleCollision(Entity *entity);
};