#include "Event.h"
#include "Entity.h"

class ReplayEvent final : public Event {
public:
    explicit ReplayEvent(Entity* entity): _entity(new Entity(*entity)) {}

    ReplayEvent(const ReplayEvent& other) : _entity(new Entity(*other._entity)) {}

    EventType getType() const override { return EventType::Replay; }

    Entity* getEntity() const { return _entity; }
    void setEntity(Entity* entity) { _entity = entity; }

    bool isReplay() const { return _isReplay; }
    void setIsReplay(const bool isReplay) { _isReplay = isReplay; }

private:
    Entity* _entity;
    bool _isReplay = false;
};