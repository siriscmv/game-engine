#include "Event.h"
#include "Entity.h"

class CollisionEvent final : public Event {
    public:
    explicit CollisionEvent(Entity* entityA, Entity* entityB)
        : _entityA(entityA), _entityB(entityB) {}
    
    EventType getType() const override { return EventType::Collision; }

    Entity* getEntityA() const { return _entityA; }
    Entity* getEntityB() const { return _entityB; }

private:
    Entity* _entityA;
    Entity* _entityB;
};
