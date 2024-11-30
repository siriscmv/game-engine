#include "Event.h"
#include "Entity.h"

class ReplayEvent final : public Event {
public:
    explicit ReplayEvent(const EntityUpdateEvent* event)
        : Event(event->getTimestamp()), _entity(new Entity(*event->getEntity())), _isReplay(false) {}

    ReplayEvent(const ReplayEvent& other)
        : Event(other.getTimestamp()), _entity(other.getEntity()), _isReplay(other.isReplay()) {}

    EventType getType() const override { return EventType::Replay; }

    Entity* getEntity() const { return _entity; }
    void setEntity(Entity* entity) { _entity = entity; }

    bool isReplay() const { return _isReplay; }
    void setIsReplay(const bool isReplay) { _isReplay = isReplay; }

private:
    Entity* _entity;
    bool _isReplay;
};