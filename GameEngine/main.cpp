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
	platform.setEntityType(EntityType::FIXED);

	std::vector<Entity*> playerEntities;
	playerEntities.push_back(&playerOne);
	playerEntities.push_back(&playerTwo);
	playerEntities.push_back(&playerThree);

	std::vector<Entity*> worldEntities;
	worldEntities.push_back(&platform);

	// Creating the server with the world entities and player entities.
	Server server(worldEntities, playerEntities);
	//FIXME: @cyril
	server.getGameEngine()->getPhysicsSystem()->applyPhysics(playerOne, 9);
	server.getGameEngine()->getPhysicsSystem()->applyPhysics(playerTwo, 9);
	server.getGameEngine()->getPhysicsSystem()->applyPhysics(playerThree, 9);
	// server.getGameEngine()->getPhysicsSystem()->applyPhysics(platform, 0);

	// Initializing the server and simulating the game world in the server
	server.initialize();
	server.run();

	return 0;
}