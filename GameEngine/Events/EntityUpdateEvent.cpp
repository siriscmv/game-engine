#include "Event.h"
#include "Entity.h"

class EntityUpdateEvent final : public Event {
public:
    explicit EntityUpdateEvent(Entity* entity): _entity(entity) {}

    EventType getType() const override { return EventType::EntityUpdate; }

    Entity* getEntity() const { return _entity; }

private:
    Entity* _entity;
};