#include "GameEngine.h"

int main(int argc, char** argv) {
	GameEngine gameEngine("CSC 581 Game Engine", 1920, 1080);

	// Initiliazing the engine
	if (!gameEngine.initialize()) {
		return -1;
	}

	// Running the game loop
	gameEngine.run();

	// Shutting down the engine
	gameEngine.shutdown();

	return 0;
}