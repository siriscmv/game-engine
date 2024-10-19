#include "GameEngine.h"
#include "Entity.h"
#include "Server.h"

// Use this file to experiment with the Engine
int main(int argc, char** argv) {

	// Player spawn points
	Entity spawnPointOne(Position(0, 0), Size(200, 200));
	Entity spawnPointTwo(Position(120, 0), Size(200, 200));
	spawnPointOne.setEntityType(EntityType::GHOST);
	spawnPointTwo.setEntityType(EntityType::GHOST);

	// Game world 
	Entity platform(Position(0, 600), Size(1920, 50));
	Entity obstacle(Position(800, 100), Size(200, 200));
	platform.setEntityType(EntityType::FIXED);

	std::vector<Entity*> spawnPoints;
	spawnPoints.push_back(&spawnPointOne);
	spawnPoints.push_back(&spawnPointTwo);

	std::vector<Entity*> worldEntities;

	int entities = 1000;

	for (int i = 0; i < entities; i++) {
		Entity entity(Position(0, i), Size(100, 100));
		entity.setEntityType(EntityType::GHOST);
		worldEntities.push_back(&entity);
	}

	// Creating the server with the world entities and player entities.
	Server server(worldEntities, spawnPoints);

	server.setRefreshRate(RefreshRate::TWO_FORTY_FPS);
	server.setSimulationSpeed(1);

	for (auto *e: worldEntities) {
		server.getGameEngine()->getPhysicsSystem()->applyPhysics(*e, 10);
	}
	
	// Applying physics
	server.getGameEngine()->getPhysicsSystem()->applyPhysics(obstacle, 0);

	// Initializing the server and simulating the game world in the server
	server.initialize();
	server.run();

	return 0;
}