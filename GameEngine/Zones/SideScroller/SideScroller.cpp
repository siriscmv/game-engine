//
// Created by Cyril Melvin Vincent on 10/9/24.
//

#include "SideScroller.h"

#include <CollisionSystem.h>

void SideScroller::addNewZone(const std::string& name, const std::tuple<Entity, std::function<void(Entity*, std::vector<Entity*>& entities)> >& zone) {
    if (_zones.find(name) != _zones.end()) {
        throw std::invalid_argument("This Side scroller zone already exists");
    }

    _zones.insert(std::make_pair(name, zone));
}

void SideScroller::removeZone(const std::string& name) {
    if (_zones.find(name) == _zones.end()) {
        throw std::invalid_argument("This Side scroller zone does not exist");
    }

    _zones.erase(name);
}

void SideScroller::process(Entity* entity, std::vector<Entity*>& entities) const {
    for (const auto & _zone : _zones) {
        auto zone = _zone.second;
        if (CollisionSystem::getInstance().hasCollision(&std::get<0>(zone), entity)) {
            std::get<1>(zone)(entity, entities);
        }
    }
}

void translate(const std::vector<Entity*>& entities, const float dx, const float dy) {
    for (const auto entity : entities) {
        entity->setOriginalPosition(
            Position(entity->getOriginalPosition().x + dx, entity->getOriginalPosition().y + dy)
        );
    }
}

void top(Entity* entity, const std::vector<Entity*>& entities) {
    translate(entities, 0.0f, -50.0f);
}

void bottom(Entity* entity, const std::vector<Entity*>& entities) {
    translate(entities, 0.0f, 50.0f);
}

void left(Entity* entity, const std::vector<Entity*>& entities) {
    translate(entities, -50.0f, 0.0f);
}

void right(Entity* entity, const std::vector<Entity*>& entities) {
    translate(entities, 50.0f, 0.0f);
}