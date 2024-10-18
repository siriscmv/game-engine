//
// Created by Cyril Melvin Vincent on 10/9/24.
//

#include "SideScroller.h"

#include <CollisionSystem.h>

void SideScroller::addNewZone(const std::string& name, const std::tuple<Entity, std::function<void(Entity*, std::vector<Entity*>& entities)>>& zone) {
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

        if (CollisionSystem::getInstance().hasCollisionRaw(&std::get<0>(zone), entity)) {
            if (std::get<1>(zone) == ZoneType::DEATH) {
                // Teleport entity to oroginal spawn position
				entity->resetToOriginalPosition();
                }
            }
            else {
                std::vector<Entity*> filteredEntities;
                std::copy_if(entities.begin(), entities.end(), std::back_inserter(filteredEntities), [entity](const Entity* e) { return entity != e; });
                std::get<1>(zone)(entity, filteredEntities);
            }
        }
    }
}