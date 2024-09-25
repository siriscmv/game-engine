#include "GameEngine.h"
#include "Entity.h"
#include "Server.h"
#include "PeerServer.h"

// Use this file to experiment with the Engine
int main(int argc, char** argv) {
	
	// Player spawns
	Entity playerOne(Position(50, 250), Size(50, 50));
	Entity playerTwo(Position(200, 350), Size(50, 50));
	Entity playerThree(Position(400, 550), Size(50, 50));

	playerOne.setEntityID(10001);
	playerTwo.setEntityID(10002);
	playerThree.setEntityID(10003);

	// Game world 
	Entity platform(Position(0, 600), Size(1920, 50));
	platform.setEntityType(EntityType::FIXED);

	std::vector<Entity*> entities;
	entities.push_back(&playerOne);
	entities.push_back(&playerTwo);
	entities.push_back(&playerThree);
	entities.push_back(&platform);

	PeerServer peerServer(entities);
	peerServer.initialize();

	while (true) {
		peerServer.run();
	}

	return 0;
}