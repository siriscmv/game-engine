#pragma once
#include "InputManager.h"
#include "Window.h"
#include "Renderer.h"


enum class GameState {PLAY, EXIT};   // A state enum class to manage a simple state machine

// Class, functions, variables signatures of the Game Engine class. This class delegates work to 
// other subsystem classes and their functions. Responsible for managing the game loop.
class GameEngine {
public:
	GameEngine(const char* windowTitle, int windowWidth, int windowHeight);
	~GameEngine();

	bool initialize();
	void run();
	void shutdown();

	InputManager * getInputManager();
	GameState getGameState();
	void setGameState(GameState state);
	

private:
	Window* _window;
	Renderer* _renderer;	
	GameState _gameState;
	InputManager* _inputManager;
};

