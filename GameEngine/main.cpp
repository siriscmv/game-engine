#include "GameEngine.h"

int main(int argc, char** argv) {
	GameEngine gameEngine("CSC 581 Game Engine", 1920, 1080);

	// Initializing the engine
	if (!gameEngine.initialize()) {
		return -1;
	}

	// Define bindings for key presses
	const std::function<void()> exitCallback = [&gameEngine]() {
		gameEngine.setGameState(GameState::EXIT);
	};

	gameEngine.getInputManager()->bind(SDL_SCANCODE_ESCAPE, "esc_exit", exitCallback);
	gameEngine.getInputManager()->bind(SDL_SCANCODE_Q, "q_exit", exitCallback);

	// Running the game loop
	gameEngine.run();

	// Shutting down the engine
	gameEngine.shutdown();

	return 0;
}