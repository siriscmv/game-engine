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

private:
    std::unordered_map<std::string, std::tuple<Entity, std::function<void(Entity*, std::vector<Entity*>& entities)>>> _zones;
};

void translate(const std::vector<Entity *>&, float, float);

void top(Entity*, const std::vector<Entity*>&);
void bottom(Entity*, const std::vector<Entity*>&);
void left(Entity*, const std::vector<Entity*>&);
void right(Entity*, const std::vector<Entity*>&);
