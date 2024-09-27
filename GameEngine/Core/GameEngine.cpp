#include "GameEngine.h"

#include <iostream>
#include <thread>
#ifdef __APPLE__
#include <SDL2/SDL.h>
#else
#include <SDL/SDL.h>
#include <vector>
#include <iostream>
#endif

// Constructor. Creates subsytems objects with parameters passed in by the user.
GameEngine::GameEngine(const char* windowTitle, int windowWidth, int windowHeight, Mode mode) {
	_mode = mode;
	_window = new Window(windowTitle, windowWidth, windowHeight);
	_renderer = new Renderer();
	_gameState = GameState::PLAY;
	_inputManager = new InputManager();
	_physicsSystem = &PhysicsSystem::getInstance();
	_collisionSystem = &CollisionSystem::getInstance();
	_onCycle = []() {};

	if (mode == Mode::CLIENT) _client = new Client();
	if (mode == Mode::PEER) _peer = new Peer();	
}

GameEngine::~GameEngine() {
	delete _renderer;
	delete _window;
	delete _inputManager;	
}

// Initializes the game engine subsystems.
bool GameEngine::initialize(std::vector<Entity*>& entities) {
	_entities = entities;	

	try {
		switch (_mode) {
			// Server initialization: Requires only physics
			case Mode::SERVER:				
				if (!_physicsSystem->initialize()) 
					throw std::runtime_error("Failed to initialize Physics System for Server");

			case Mode::PEER_SERVER:
				_peerServer->initialize();

				break;
			// Client initialization: Requires window and renderer
			case Mode::CLIENT:
				if (_client) {
					_client->initialize();					
					if (!_client->handshakeWithServer()) {						
						throw std::runtime_error("Failed to connect to the server");
					}
					_entities = _client->getEntities();
				}
				if (!_window->initialize()) {
					throw std::runtime_error("Failed to initialize Window for Client");
				}
				if (!_renderer->initialize(_window->getSDLWindow())) {
					throw std::runtime_error("Failed to initialize Renderer for Client");
				}			
				break;
		
			case Mode::PEER:
				if (_peer) {
					_peer->initialize();
					if (!_peer->handshakeWithServer()) {
						throw std::runtime_error("Failed to connect to the server");
					}
					_entities = _peer->getEntities();
				}
				if (!_window->initialize()) {
					throw std::runtime_error("Failed to initialize Window for Peer");
				}
				if (!_renderer->initialize(_window->getSDLWindow())) {
					throw std::runtime_error("Failed to initialize Renderer for Peer");
				}
				break;
			// Single Player initialization: Requires all systems 
			case Mode::SINGLE_PLAYER:			
				if (!_window->initialize()) {
					throw std::runtime_error("Failed to initialize Window for Single Player/Peer");
				}
				if (!_renderer->initialize(_window->getSDLWindow())) {
					throw std::runtime_error("Failed to initialize Renderer for Single Player/Peer");
				}
				if (!_physicsSystem->initialize()) {
					throw std::runtime_error("Failed to initialize Physics System for Single Player/Peer");
				}
				break;
			default:
				throw std::runtime_error("Unrecognized mode during initialization.");
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Initialization Error: " << e.what() << std::endl;
		return false;
	}
	
	return true;
}

// Game loop. Runs while the state is 'PLAY'.
void GameEngine::run() {
	while (_gameState == GameState::PLAY) {
		switch (_mode) {
			case Mode::SERVER:
				handleServerMode();
				break;
			case Mode::PEER_SERVER:
				handlePeerServerMode();
				break;
			case Mode::CLIENT:
				handleClientMode();
				break;
			case Mode::PEER:
				handlePeerToPeerMode();
				break;
			case Mode::SINGLE_PLAYER:
				handleSinglePlayerMode();
				break;			
		}		
	}
}

// Handles the server's game engine logic in server-client multiplayer
void GameEngine::handleServerMode() {
	std::set<Entity*> entitiesWithCollisions = _collisionSystem->run(_entities);        // Running the collision system
	_physicsSystem->run(0.1f, entitiesWithCollisions);                                  // Running the physics engine
	SDL_Delay(16);
}

void GameEngine::handlePeerServerMode() {
	_peerServer->run();
}

// Handles the client's game engine logic in server-client multiplayer
void GameEngine::handleClientMode() {
	SDL_PumpEvents();                                          // Force an event queue update
	_renderer->clear();

	std::thread inputThread([this]() {
		_inputManager->process();
	});

	std::thread callbackThread([this]() {
		_onCycle();
	});

	std::thread communicationThread([this]() {
		_client->receiveUpdatesFromServer();
	});
	
	auto [scaleX, scaleY] = _window->getScaleFactors();

	// Rendering all entities
	for (Entity* entity : _entities) {
		entity->applyScaling(scaleX, scaleY);
		entity->render(_renderer->getSDLRenderer());             // Rendering all entities
	}

	_renderer->present();

	inputThread.join();
	callbackThread.join();
	communicationThread.join();

	SDL_Delay(16);                                                // Setting 60hz refresh rate
}

void GameEngine::handlePeerToPeerMode() {
	SDL_PumpEvents();
	_renderer->clear();

	std::thread inputThread([this]() {
		_inputManager->process();
		});

	std::thread callbackThread([this]() {
		_onCycle();
		});

	std::thread communicationThread([this]() {
		_peer->receiveUpdates();		
		});

	// Run physics for own entity
	int assignedEntityID = _peer->_entityID; 
	auto predicate = [assignedEntityID](const Entity* entity) {
		return entity->getEntityID() == assignedEntityID;
		};
	std::vector<Entity*> filteredEntities;
	std::copy_if(_entities.begin(), _entities.end(), std::back_inserter(filteredEntities), predicate);

	std::set<Entity*> entitiesWithCollisions = _collisionSystem->run(_entities);
	_physicsSystem->runForGivenEntities(0.1f, entitiesWithCollisions, filteredEntities);

	auto [scaleX, scaleY] = _window->getScaleFactors();

	// Rendering all entities
	for (Entity* entity : _entities) {
		entity->applyScaling(scaleX, scaleY);
		entity->render(_renderer->getSDLRenderer());
	}

	_renderer->present();

	_peer->broadcastUpdates();

	inputThread.join();
	callbackThread.join();
	communicationThread.join();

	SDL_Delay(16);
}
	
// Handles the singleplayer game engine logic.
void GameEngine::handleSinglePlayerMode() {	
	SDL_PumpEvents();                                             // Force an event queue update
	_renderer->clear();

	std::thread inputThread([this]() {
		_inputManager->process();
	});

	std::thread callbackThread([this]() {
		_onCycle();
	});

	std::thread physicsThread([this]() {
		std::set<Entity*> entitiesWithCollisions = _collisionSystem->run(_entities);        // Running the collision system
		_physicsSystem->run(0.1f, entitiesWithCollisions);
	});

	auto [scaleX, scaleY] = _window->getScaleFactors();

	// Rendering all entities
	for (Entity* entity : _entities) {
		entity->applyScaling(scaleX, scaleY);
		entity->render(_renderer->getSDLRenderer());              // Rendering all entities
	}

	_renderer->present();

	inputThread.join();
	callbackThread.join();
	physicsThread.join();

	SDL_Delay(16);                                                // Setting 60hz refresh rate
}

// Sends the user input to server 
void GameEngine::sendInputToServer(const std::string& buttonPress) {
	_client->sendInputToServer(buttonPress);
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

// Getters
InputManager* GameEngine::getInputManager() { return _inputManager; }
GameState GameEngine::getGameState() { return _gameState; }
PhysicsSystem* GameEngine::getPhysicsSystem() { return _physicsSystem; }
Client* GameEngine::getClient() { return _client; }
Peer* GameEngine::getPeer() { return _peer; }

// Setters
void GameEngine::setGameState(GameState state) { _gameState = state; }

// Runs on each game cycle
void GameEngine::setOnCycle(const std::function<void()> &cb) {
	_onCycle = cb;
}

