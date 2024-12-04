#pragma once

#include "Event.h"
#include "Entity.h"

class EntityUpdateEvent final : public Event {
public:
    explicit EntityUpdateEvent(Entity* entity): _entity(new Entity(*entity)) {}

    EntityUpdateEvent(const EntityUpdateEvent& other) : _entity(new Entity(*other._entity)) {}

    EventType getType() const override { return EventType::EntityUpdate; }

    Entity* getEntity() const { return _entity; }
    void setEntity(Entity* entity) { _entity = entity; }

    bool isReplay() const { return _isReplay; }
    void setIsReplay(const bool isReplay) { _isReplay = isReplay; }

private:
    Entity* _entity;
    bool _isReplay = false;
};