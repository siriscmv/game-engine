//
// Created by Cyril Melvin Vincent on 10/9/24.
//

#pragma once
#include <Entity.h>
#include "unordered_map"
#include "tuple"
#include "functional"

class SideScroller {
public:
    void addNewZone(const std::string &name, const std::tuple<Entity, std::function<void(Entity *, std::vector<Entity*>& entities)>> &zone);
    void removeZone(const std::string &name);
    void process(Entity* entity, std::vector<Entity *> &entities) const;
    void enableDefaultSideScrollerZone(DefaultSideScrollerZone zone);

private:
    std::unordered_map<std::string, std::tuple<Entity, std::function<void(Entity*, std::vector<Entity*>& entities)>>> _zones;
};
