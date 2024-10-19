#include "GameEngine.h"
#include "Entity.h"
#include "Server.h"

// Use this file to experiment with the Engine
int main(int argc, char** argv) {

	// Player spawn points
	Entity spawnPointOne(Position(0, 100), Size(200, 200));
	Entity spawnPointTwo(Position(1520, 100), Size(200, 200));
	spawnPointOne.setEntityType(EntityType::GHOST);
	spawnPointOne.setZoneType(ZoneType::SPAWN);
	spawnPointTwo.setEntityType(EntityType::GHOST);
	spawnPointTwo.setZoneType(ZoneType::SPAWN);

	// Death zone
	Entity deathZoneOne(Position(500, 500), Size(100, 100));
	deathZoneOne.setEntityType(EntityType::GHOST);
	deathZoneOne.setZoneType(ZoneType::DEATH);

	// Side boundaries
	Entity leftBoundary(Position(0, 0), Size(50, 1080));
	leftBoundary.setEntityType(EntityType::GHOST);
	leftBoundary.setZoneType(ZoneType::SIDESCROLL);
	Entity rightBoundary(Position(1870, 0), Size(50, 1080));
	rightBoundary.setEntityType(EntityType::GHOST);
	rightBoundary.setZoneType(ZoneType::SIDESCROLL);
	Entity topBoundary(Position(0, 0), Size(1920, 50));
	topBoundary.setEntityType(EntityType::GHOST);
	topBoundary.setZoneType(ZoneType::SIDESCROLL);
	Entity bottomBoundary(Position(0, 1030), Size(1920, 50));
	bottomBoundary.setEntityType(EntityType::GHOST);
	bottomBoundary.setZoneType(ZoneType::SIDESCROLL);

	// Game world 
	Entity platform(Position(0, 600), Size(1920, 50));
	Entity obstacle(Position(800, 100), Size(200, 200));
	Entity obstacleTwo(Position(2000, 400), Size(200, 200));                // Off screen object
	platform.setEntityType(EntityType::FIXED);

	std::vector<Entity*> entities;
	entities.push_back(&spawnPointOne);
	entities.push_back(&spawnPointTwo);
	entities.push_back(&deathZoneOne);
	entities.push_back(&leftBoundary);
	entities.push_back(&rightBoundary);
	entities.push_back(&topBoundary);
	entities.push_back(&bottomBoundary);
	entities.push_back(&platform);
	entities.push_back(&obstacle);
	entities.push_back(&obstacleTwo);

	// Creating the server with the world entities and player entities.
	Server server(entities);

	server.setRefreshRate(RefreshRate::TWO_FORTY_FPS);
	server.setSimulationSpeed(1);
	
	// Applying physics
	server.getGameEngine()->getPhysicsSystem()->applyPhysics(obstacle, 0);
	server.getGameEngine()->getPhysicsSystem()->applyPhysics(obstacleTwo, 0);

	// Initializing the server and simulating the game world in the server
	server.initialize();
	server.run();

	return 0;
}