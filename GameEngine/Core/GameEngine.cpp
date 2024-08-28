#include "GameEngine.h"
#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL/SDL.h>
#endif

// Constructor. Creates subsytems objects with parameters passed in by the user.
GameEngine::GameEngine(const char* windowTitle, int windowWidth, int windowHeight) {
	_window = new Window(windowTitle, windowWidth, windowHeight);
	_renderer = new Renderer();
	_gameState = GameState::PLAY;	
}

GameEngine::~GameEngine() {
	delete _renderer;
	delete _window;
}

// Initializes the game engine subsystems.
bool GameEngine::initialize() {
	if (!_window->initialize()) {
		return false;
	}
	if (!_renderer->initialize(_window->getSDLWindow())) {
		return false;
	}
	return true;
}

// Game loop. Runs while the state is 'PLAY'.
void GameEngine::run() {
	SDL_Event currentEvent;

	while (_gameState == GameState::PLAY) {
		_renderer->clear();
		_renderer->present();

		while (SDL_PollEvent(&currentEvent)) {
			switch (currentEvent.type) {
				case SDL_QUIT:
					_gameState = GameState::EXIT;
					break;
				}
		}
	}
}

// Quits the game engine. Destroys all objects
void GameEngine::shutdown() {
	_renderer->shutdown();
	_window->shutdown();
	SDL_Quit();
}
