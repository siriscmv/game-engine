#include "PhysicsSystem.h"

// Initializes class variables
bool PhysicsSystem::initialize() {	
	_entities.clear();	
	return true;
}

// Sets velocity, acceleration (including gravity) for the Entity that passed into this function
void PhysicsSystem::applyPhysics(Entity& entity, float gravity, Velocity velocity, Acceleration acceleration) {
	entity.setVelocityX(velocity.x);
	entity.setVelocityY(velocity.y);
	entity.setAccelerationX(acceleration.x);
	entity.setAccelerationY(acceleration.y + gravity);
	
	// Add the entity to the list if it's not already there
	if (std::find(_entities.begin(), _entities.end(), &entity) == _entities.end()) {
		_entities.push_back(&entity);
	}
}

// Similates the physics system. All entities included in the _entities array will experience physics.
void PhysicsSystem::run(float deltaTime, std::set<Entity*>& entitiesToIgnore) {
	for (Entity* entity : _entities) {
		if (entitiesToIgnore.find(entity) == entitiesToIgnore.end()) {
			// Updating velocity with acceleration (v = u + at)
			entity->setVelocityX(entity->getVelocityX() + entity->getAccelerationX() * deltaTime);
			entity->setVelocityY(entity->getVelocityY() + entity->getAccelerationY() * deltaTime);

			// Updating position with velocity (s = s0 + vt)
			entity->setPosition(Position(
				entity->getPosition().x + entity->getVelocityX() * deltaTime,
				entity->getPosition().y + entity->getVelocityY() * deltaTime
			));
		}
	}	
}

// Clears up the entities variable
void PhysicsSystem::shutdown() {
	_entities.clear();
}
