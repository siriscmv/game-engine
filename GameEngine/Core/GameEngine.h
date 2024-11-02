#pragma once

#include "PeerServer.h"
#include "Peer.h"
#include "CollisionSystem.h"
#include "InputManager.h"
#include "Window.h"
#include "Renderer.h"
#include "PhysicsSystem.h"
#include "Globals.h"
#include "Client.h"
#include "../TimeSystem/Timeline.h"


// Class, functions, variables signatures of the Game Engine class. This class delegates work to 
// other subsystem classes and their functions. Responsible for managing the game loop.
class GameEngine {
public:
	GameEngine(const char* windowTitle, int windowWidth = 1920, int windowHeight = 1080, Mode mode = Mode::SINGLE_PLAYER);
	~GameEngine();

	bool initialize(std::vector<Entity*>& entities);
	void run();
	void shutdown();

	InputManager * getInputManager();
	GameState getGameState();
	Client* getClient();

	Peer *getPeer();

	void setGameState(GameState state);
	// A callback function that is triggered after each render cycle
	void setOnCycle(const std::function<void()> &);
	PhysicsSystem* getPhysicsSystem();
	CollisionSystem* getCollisionSystem();
	Window* getWindow();

	void toggleScalingMode();
	void sendInputToServer(const std::string& buttonPress);

	void pauseGame();
	void resumeGame();
	void setGameSpeed(double speed);
	void setClientMap(std::map<int, Entity*>& clientMap);
	
	int getServerRefreshRateMs() const;
	void setServerRefreshRateMs(int rate);

	std::vector<Entity*>& getEntities();

	void initializeCamera(int width, int height);
	void resizeCamera(float scaleX, float scaleY);
	void moveCamera(int x, int y);
	Camera& getCamera();

private:
	Window* _window;
	Renderer* _renderer;	
	GameState _gameState;
	Mode _mode;
	InputManager* _inputManager;
	PhysicsSystem* _physicsSystem;
	CollisionSystem* _collisionSystem;
	std::vector<Entity*> _entities;
	std::function<void()> _onCycle;
	Timeline* _timeline;

	Client* _client = nullptr;
	Peer* _peer = nullptr;
	std::map<int, Entity*>* _clientMap;
	
	void handleServerMode(int64_t elapsedTime);	

	void handleClientMode(int64_t elapsedTime);
	void handlePeerToPeerMode(int64_t elapsedTime);
	void handleSinglePlayerMode(int64_t elapsedTime);

	int _serverRefreshRateMs;

	Camera _camera;
};

