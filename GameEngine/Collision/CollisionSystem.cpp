#include  "CollisionSystem.h"

#include <functional>
#include <stdexcept>

#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL/SDL.h>
#endif

bool CollisionSystem::hasCollision(const Entity *entityA, const Entity *entityB) {
    if (entityA->getShapeType() != ShapeType::RECTANGLE || entityB->getShapeType() != ShapeType::RECTANGLE) {
        throw std::runtime_error("Unsupported entity types for collision detection");
    }

    if (entityB->getEntityType() == EntityType::FIXED && entityA->getEntityType() != EntityType::FIXED) {
        // Ensure that a fixed entity type is used as the first param
        return hasCollision(entityB, entityA);
    }

    // Helper function to convert a rectangle entity to a SDL_Rect
    const std::function<const SDL_Rect*(const Entity&)> to_rect = [](const Entity& entity) {
        auto* rect = new SDL_Rect{
            static_cast<int>(entity.getPosition().x), static_cast<int>(entity.getPosition().y),
            static_cast<int>(entity.getSize().width), static_cast<int>(entity.getSize().height)
        };
        return rect;
    };

    if (entityA->getEntityType() == EntityType::FIXED) {
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

std::set<Entity *> CollisionSystem::run(const std::vector<Entity *>& entities) {
    const auto n = entities.size();
    std::set<Entity *> collisions;

    for (int i = 0; i < n; i++) {
        for (int j = i+1; j < n; j++) {
            Entity *entityA = entities[i];
            Entity* entityB = entities[j];

            if (getInstance().hasCollision(entityA, entityB)) {
                applyPhysics(entityA);
                applyPhysics(entityB);

                collisions.insert(entityA);
                collisions.insert(entityB);
            }

        }
    }

    return collisions;
}

void CollisionSystem::applyPhysics(Entity *entity) {
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
        default:
            break;
    }
}
