#include "Event.h"
#include "Entity.h"

class SpawnEvent final : public Event {
public:
    explicit SpawnEvent(Entity* entity, const Position& position)
        : _entity(entity), _position(position) {}

    EventType getType() const override { return EventType::Spawn; }

    Entity* getEntity() const { return _entity; }

    Position getPosition() const { return _position; }

private:
    Entity* _entity;
    Position _position;
};