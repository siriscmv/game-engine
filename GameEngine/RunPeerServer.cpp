#include "GameEngine.h"
#include "Entity.h"
#include "Server.h"
#include "PeerServer.h"

// Use this file to experiment with the Engine
int main(int argc, char** argv) {
	
	// Player spawns
	Entity playerOne(Position(50, 250), Size(50, 50));
	Entity playerTwo(Position(200, 350), Size(50, 50));
	Entity playerThree(Position(400, 550), Size(50, 55));	

	playerOne.setAccelerationY(10.0f);
	playerTwo.setAccelerationY(10.0f);
	playerThree.setAccelerationY(10.0f);

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

	PeerServer peerServer(worldEntities, playerEntities);
	peerServer.initialize();

	while (true) {
		peerServer.run();
	}

	return 0;
}