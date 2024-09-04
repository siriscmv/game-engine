#include "GameEngine.h"
#include "Entity.h"

// Use this file to experiment with the Engine
int main(int argc, char** argv) {
	GameEngine gameEngine("CSC 581 Game Engine");

	Entity rect(Position(500, 500), Size(100, 100));
	Entity platform(Position(0, 800), Size(1080, 20));

	platform.setEntityType(EntityType::FIXED);

	// Create an array of entities
	std::vector<Entity*> entities;
	entities.push_back(&rect);
	entities.push_back(&platform);

	// Initializing the engine
	if (!gameEngine.initialize(entities)) {
		return -1;
	}

	gameEngine.getPhysicsSystem()->applyPhysics(rect, 0);

	// Define bindings for key presses
	const std::function<void()> exitCallback = [&gameEngine]() {
		gameEngine.setGameState(GameState::EXIT);
	};

	// Key bind for screen scaling mode flip
	gameEngine.getInputManager()->bind(SDL_SCANCODE_S, "toggle_scaling", [&gameEngine]() {
		gameEngine.toggleScalingMode();
		});

	
	// Key binds for entity movement
	gameEngine.getInputManager()->bind(SDL_SCANCODE_LEFT, "move_left", [&rect]() { rect.setVelocityX(-50.f); });
	gameEngine.getInputManager()->bind(SDL_SCANCODE_RIGHT, "move_right", [&rect]() { rect.setVelocityX(50.0f); });
	gameEngine.getInputManager()->bind(SDL_SCANCODE_UP, "move_up", [&rect]() { rect.setVelocityY(-50.0f); });
	gameEngine.getInputManager()->bind(SDL_SCANCODE_DOWN, "move_down", [&rect]() { rect.setVelocityY(50.0f); });
	
	// Key binds for exiting the game engine
	gameEngine.getInputManager()->bind(SDL_SCANCODE_ESCAPE, "esc_exit", exitCallback);
	gameEngine.getInputManager()->bind(SDL_SCANCODE_Q, "q_exit", exitCallback);

	// Running the game loop
	gameEngine.run();

	// Shutting down the engine
	gameEngine.shutdown();

	return 0;
}