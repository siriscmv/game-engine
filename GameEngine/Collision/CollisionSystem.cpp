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

    // Helper function to convert a rectangle entity to a SDL_Rect
    const std::function<const SDL_Rect*(const Entity&)> to_rect = [](const Entity& entity) {
        SDL_Rect* rect = new SDL_Rect{entity.getPosition().x, entity.getPosition().y, entity.getSize().width, entity.getSize().height};
        return rect;
    };

    return SDL_HasIntersection(to_rect(*entityA), to_rect(*entityB));
}

void CollisionSystem::run(const std::vector<Entity *>& entities) {
    const auto n = entities.size();

    for (int i = 0; i < n; i++) {
        for (int j = i+1; j < n; j++) {
            Entity *entityA = entities[i];
            Entity* entityB = entities[j];

            if (hasCollision(entityA, entityB)) {
                applyPhysics(entityA);
                applyPhysics(entityB);
            }

        }
    }
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
