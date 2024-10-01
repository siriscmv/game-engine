#include "GameEngine.h"
#include "Entity.h"
#include "Server.h"

// Use this file to experiment with the Engine
int main(int argc, char** argv) {

	// Player spawns
	Entity playerOne(Position(50, 250), Size(50, 50));
	Entity playerTwo(Position(200, 350), Size(50, 50));
	Entity playerThree(Position(400, 550), Size(50, 50));

	// Game world 
	Entity platform(Position(0, 600), Size(1920, 50));
	Entity obstacle(Position(800, 100), Size(200, 200));
	platform.setEntityType(EntityType::FIXED);

	std::vector<Entity*> playerEntities;
	playerEntities.push_back(&playerOne);
	playerEntities.push_back(&playerTwo);
	playerEntities.push_back(&playerThree);

	std::vector<Entity*> worldEntities;
	worldEntities.push_back(&platform);
	worldEntities.push_back(&obstacle);

	// Creating the server with the world entities and player entities.
	Server server(worldEntities, playerEntities);

	server.setRefreshRate(RefreshRate::TWO_FORTY_FPS);
	server.setSimulationSpeed(2);
	//FIXME: @cyril
	server.getGameEngine()->getPhysicsSystem()->applyPhysics(playerOne, 9.8f);
	server.getGameEngine()->getPhysicsSystem()->applyPhysics(playerTwo, 9.8f);
	server.getGameEngine()->getPhysicsSystem()->applyPhysics(playerThree, 9.8f);
	server.getGameEngine()->getPhysicsSystem()->applyPhysics(obstacle, 0);

	// Initializing the server and simulating the game world in the server
	server.initialize();	

	server.run();

	return 0;
}