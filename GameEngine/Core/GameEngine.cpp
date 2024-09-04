#include "GameEngine.h"
#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL/SDL.h>
#include <vector>
#endif

// Constructor. Creates subsytems objects with parameters passed in by the user.
GameEngine::GameEngine(const char* windowTitle, int windowWidth, int windowHeight) {
	_window = new Window(windowTitle, windowWidth, windowHeight);
	_renderer = new Renderer();
	_gameState = GameState::PLAY;
	_inputManager = new InputManager();
	_physicsSystem = &PhysicsSystem::getInstance();
	_collisionSystem = &CollisionSystem::getInstance();
}

GameEngine::~GameEngine() {
	delete _renderer;
	delete _window;
}

// Initializes the game engine subsystems.
bool GameEngine::initialize(std::vector<Entity*>& entities) {
	_entities = entities;	

	if (!_window->initialize()) {
		return false;
	}
	if (!_renderer->initialize(_window->getSDLWindow())) {
		return false;
	}
	if (!_physicsSystem->initialize()) {
		return false;
	}
	return true;
}

// Game loop. Runs while the state is 'PLAY'.
void GameEngine::run() {	

	while (_gameState == GameState::PLAY) {
		// Force an event queue update, otherwise events will not be placed in the queue
		SDL_PumpEvents();
		_renderer->clear();

		
		auto [scaleX, scaleY] = _window->getScaleFactors();

		// Rendering all entities
		for (Entity* entity : _entities) {
			entity->applyScaling(scaleX, scaleY);
			entity->render(_renderer->getSDLRenderer());             // Rendering all entities
		}

		_physicsSystem->run(0.1f);                                   // Running the physics engine
		_collisionSystem->run(_entities);                            // Running the collision system

		_renderer->present();
		_inputManager->process();		

		SDL_Delay(16);                                                // Setting 60hz refresh rate
	}
}

// Quits the game engine. Destroys all objects
void GameEngine::shutdown() {
	_renderer->shutdown();
	_window->shutdown();

	for (Entity* entity : _entities) {
		entity->shutdown();
	}

	_physicsSystem->shutdown();
	SDL_Quit();
}

// Toggles the scaling mode variable. Delegates to Window class
void GameEngine::toggleScalingMode() {
	_window->toggleScalingMode();
}

InputManager* GameEngine::getInputManager() {
	return _inputManager;
}
GameState GameEngine::getGameState() {
	return _gameState;
}

void GameEngine::setGameState(GameState state) {
	_gameState = state;
}

PhysicsSystem* GameEngine::getPhysicsSystem() {
	return _physicsSystem;
}
