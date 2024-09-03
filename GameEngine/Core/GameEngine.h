#pragma once

#include "CollisionSystem.h"
#include "InputManager.h"
#include "Window.h"
#include "Renderer.h"
#include "PhysicsSystem.h"


enum class GameState {PLAY, EXIT};   // A state enum class to manage a simple state machine

// Class, functions, variables signatures of the Game Engine class. This class delegates work to 
// other subsystem classes and their functions. Responsible for managing the game loop.
class GameEngine {
public:
	GameEngine(const char* windowTitle, int windowWidth = 1920, int windowHeight = 1080);
	~GameEngine();

	bool initialize(std::vector<Entity*>& entities);
	void run();
	void shutdown();

	InputManager * getInputManager();
	GameState getGameState();
	void setGameState(GameState state);
	PhysicsSystem* getPhysicsSystem();	

	void toggleScalingMode();	

private:
	Window* _window;
	Renderer* _renderer;	
	GameState _gameState;	
	InputManager* _inputManager;
	PhysicsSystem* _physicsSystem;
	CollisionSystem* _collisionSystem;
	std::vector<Entity*> _entities;
	int _entityCount;
};

