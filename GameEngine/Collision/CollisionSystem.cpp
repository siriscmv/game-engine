#include  "CollisionSystem.h"
#include  "CollisionEvent.cpp"
#include  "DeathEvent.cpp"

#include <functional>
#include <stdexcept>

#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL/SDL.h>
#endif

// Helper function to convert a rectangle entity to a SDL_Rect
const std::function<const SDL_Rect*(const Entity&)> to_rect = [](const Entity& entity) {
    auto* rect = new SDL_Rect{
        static_cast<int>(entity.getOriginalPosition().x), static_cast<int>(entity.getOriginalPosition().y),
        static_cast<int>(entity.getSize().width), static_cast<int>(entity.getSize().height)
    };
    return rect;
};

bool CollisionSystem::hasCollisionRaw(const Entity *entityA, const Entity *entityB) {
    if (entityA->getShapeType() != ShapeType::RECTANGLE || entityB->getShapeType() != ShapeType::RECTANGLE) {
        throw std::runtime_error("Unsupported entity types for collision detection");
    }

    const SDL_Rect* rectA = to_rect(*entityA);
    const SDL_Rect* rectB = to_rect(*entityB);

    return SDL_HasIntersection(rectA, rectB);
}

bool CollisionSystem::hasCollision(const Entity *entityA, const Entity *entityB) {
    if (entityA->getShapeType() != ShapeType::RECTANGLE || entityB->getShapeType() != ShapeType::RECTANGLE) {
        throw std::runtime_error("Unsupported entity types for collision detection");
    }

    if (entityA->getEntityType() == EntityType::GHOST || entityB->getEntityType() == EntityType::GHOST) {
        return false;
    }

    // Helper function to detect if an entity is not moving
    const std::function<bool(const Entity&)> is_not_moving = [](const Entity& entity) {
        return entity.getEntityType() == EntityType::FIXED || (entity.getVelocityX() == 0 && entity.getVelocityY() == 0);
    };

    if (is_not_moving(*entityB) && !is_not_moving(*entityA)) {
        // Ensure that a fixed entity is used as the first param
        return hasCollision(entityB, entityA);
    }

    if (is_not_moving(*entityA)) {
        // If one of the entities is fixed, and the other entity has opposing movements, then it is not technically a collision.
        // In other words, the entities are moving away from a collision

        if (entityB->getAccelerationY() * entityB->getVelocityY() < 0) {
            return false;
        }

        if (entityB->getAccelerationX() * entityB->getVelocityX() < 0) {
            return false;
        }
    }

    return SDL_HasIntersection(to_rect(*entityA), to_rect(*entityB));
}

std::set<Entity*> CollisionSystem::run(const std::vector<Entity*>& entities, EventManager* eventManager) {
    std::set<Entity*> collisions;

    // Create a local copy of the entities vector
    std::vector<Entity*> entitiesCopy = entities;

    for (auto itA = entitiesCopy.begin(); itA != entitiesCopy.end(); ++itA) {
        for (auto itB = std::next(itA); itB != entitiesCopy.end(); ++itB) {
            Entity* entityA = *itA;
            Entity* entityB = *itB;

            // Check if pointers are still valid
            if (!entityA || !entityB) {
                continue;
            }

            // Ensure both entities exist in the vector
            if (std::find(entities.begin(), entities.end(), entityA) == entities.end() ||
                std::find(entities.begin(), entities.end(), entityB) == entities.end()) {
                continue;
            }

            // Check for collisions and raise a collision event
            if (getInstance().hasCollision(entityA, entityB)) {
                eventManager->raiseEvent(new CollisionEvent(entityA, entityB));

                collisions.insert(entityA);
                collisions.insert(entityB);
            }
        }
    }

    return collisions;
}

void CollisionSystem::handleCollision(Entity *entity) {
    switch (entity->getEntityType()) {
        case EntityType::DEFAULT:
            // Unrealistic, but keeping it this way for now
            entity->setVelocityX(0);
            entity->setVelocityY(0);
            break;
        case EntityType::ELASTIC:
            entity->setVelocityX(-1 * entity->getVelocityX());
            entity->setVelocityY(-1 * entity->getVelocityY());
            break;
        case EntityType::FIXED:
        case EntityType::GHOST:
        default:
            break;
    }
}
