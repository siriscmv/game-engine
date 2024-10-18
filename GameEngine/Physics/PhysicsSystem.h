#pragma once

#include <set>

#include "Entity.h"
#include <vector>

// A singleton class that simulates a physics system. Currently handles 
// gravity, horizontal movement, and vertical movement.
class PhysicsSystem {
public:
	static PhysicsSystem& getInstance() {
		static PhysicsSystem instance;                    // Singleton variable
		return instance;
	}

	PhysicsSystem(const PhysicsSystem&) = delete;         // Preventing copying 
	void operator=(const PhysicsSystem) = delete;         // Preventing assignmenet

	// Initializes class variables
	bool initialize();

	// Applies physical attributes to the entity that is passed in
	void applyPhysics(Entity& entity, float gravity = 9.8f, Velocity velocity = Velocity(), 
		Acceleration acceleration = Acceleration());

	// Simulates physics of the entire system
	void run(float deltaTime, std::set<Entity*>& entitiesToIgnore);
	void pause();
	void resume();

	void runForGivenEntities(float deltaTime, std::set<Entity*>& entitiesToIgnore, const std::vector<Entity *> &entities);

	// Shuts the physics engine down
	void shutdown();

	std::vector<Entity*>& getEntities();
	void setEntities(const std::vector<Entity *> &entities);

private:
	PhysicsSystem() = default;
	~PhysicsSystem() = default;

	std::vector<Entity*> _entities;
	bool _isPaused = false;
};