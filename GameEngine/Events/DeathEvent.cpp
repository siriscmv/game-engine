#include "Event.h"
#include "Entity.h"

class DeathEvent final : public Event {
public:
   explicit DeathEvent(Entity* entity, const Position& respawnPosition)
        : _entity(entity), _respawnPosition(respawnPosition) {}

    EventType getType() const override { return EventType::Death; }

    Entity* getEntity() const { return _entity; }

	Position getRespawnPosition() const { return _respawnPosition; }

private:
	Entity* _entity;
	Position _respawnPosition;
};