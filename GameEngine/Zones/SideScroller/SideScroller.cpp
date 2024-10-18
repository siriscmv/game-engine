//
// Created by Cyril Melvin Vincent on 10/9/24.
//

#include "SideScroller.h"

#include <CollisionSystem.h>

void SideScroller::addNewZone(const std::string& name, const std::tuple<Entity, std::function<void(Entity*, std::vector<Entity*>& entities)>, ZoneType>& zone) {
    if (_zones.find(name) != _zones.end()) {
        throw std::invalid_argument("This Side scroller zone already exists");
    }

    if (std::get<2>(zone) == ZoneType::SPAWN) {
        _spawnPoints.push_back(std::get<0>(zone).getPosition());
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
    for (const auto& _zone : _zones) {
        auto zone = _zone.second;

        if (CollisionSystem::getInstance().hasCollisionRaw(&std::get<0>(zone), entity)) {
            if (std::get<2>(zone) == ZoneType::DEATH) {
                // Teleport entity to a random spawn point
                if (!_spawnPoints.empty()) {
                    entity->teleportTo(_spawnPoints[rand() % _spawnPoints.size()]);
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
