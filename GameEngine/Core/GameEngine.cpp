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
	_sideScroller = new SideScroller();
	_physicsSystem = &PhysicsSystem::getInstance();
	_collisionSystem = &CollisionSystem::getInstance();
	_onCycle = []() {};

	_timeline = new Timeline();

	if (mode == Mode::CLIENT) _client = new Client();
	if (mode == Mode::PEER) _peer = new Peer();	
}

GameEngine::~GameEngine() {
	delete _renderer;
	delete _window;
	delete _inputManager;
	delete _sideScroller;
	delete _timeline; 
}

// Initializes the game engine subsystems.
bool GameEngine::initialize(std::vector<Entity*>& entities) {
	_entities = entities;

	try {
		switch (_mode) {
			// Server initialization: Requires only physics and global timeline
			case Mode::SERVER:				
				if (!_physicsSystem->initialize()) 
					throw std::runtime_error("Failed to initialize Physics System for Server");
				break;			
			// Client initialization: Requires window, renderer, its own global and local timeline
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
				if (!_timeline->initialize(TimelineType::Local)) {
					throw std::runtime_error("Failed to initialize Time line for Server");
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
				if (!_timeline->initialize(TimelineType::Local)) {
					throw std::runtime_error("Failed to initialize Time line for Single Player/Peer");
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
	int64_t previousTime = _timeline->getTime();
	int64_t currentTime;
	int sleepDurationMs = 0;

	while (_gameState != GameState::EXIT) {
		
		currentTime = _timeline->getTime();
		int64_t elapsedTime = currentTime - previousTime;
		previousTime = currentTime;
		
		switch (_mode) {
		case Mode::SERVER:
			handleServerMode(elapsedTime);
			sleepDurationMs = getServerRefreshRateMs();
			break;
		case Mode::CLIENT:
			handleClientMode(elapsedTime);
			sleepDurationMs = _client->getRefreshRateMs();
			break;
		case Mode::PEER:
			handlePeerToPeerMode(elapsedTime);
			sleepDurationMs = _peer->getRefreshRateMs();
			break;
		case Mode::SINGLE_PLAYER:
			handleSinglePlayerMode(elapsedTime);
			sleepDurationMs = 1000 / static_cast<int>(RefreshRate::SIXTY_FPS);
			break;
		}
		
		int64_t sleepDurationNs = sleepDurationMs * 1e6;

		// Sleep based on frame rate (refresh rate)
		std::this_thread::sleep_for(std::chrono::nanoseconds(sleepDurationNs));
	}
}

// Handles the server's game engine logic in server-client multiplayer
void GameEngine::handleServerMode(int64_t elapsedTime) {
	_onCycle();
	std::set<Entity*> entitiesWithCollisions = _collisionSystem->run(_entities);        // Running the collision system

	float deltaTime = static_cast<float>(elapsedTime) * 1e-8f;	
 	_physicsSystem->run(deltaTime, entitiesWithCollisions);
}

// Handles the client's game engine logic in server-client multiplayer
void GameEngine::handleClientMode(int64_t elapsedTime) {
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

	std::thread sideScrollingThread([this]() {
		auto self = std::find_if(_entities.begin(), _entities.end(), [this](const Entity* e) {
			return e->getEntityID() == _client->getEntityID();
		});

		_sideScroller->process(*self, _entities);
	});
	
	auto [scaleX, scaleY] = _window->getScaleFactors();
	Position offset = _client->getViewOffset();

	// Rendering all entities
	for (Entity* entity : _entities) {
		Position pos = entity->getOriginalPosition();
		// const bool skip = entity->getEntityID() == _client->getEntityID();
		const bool skip = entity->getEntityType() == EntityType::GHOST;
		// if (!skip) entity->setOriginalPosition(Position{ pos.x - offset.x, pos.y - offset.y });

		entity->applyScaling(scaleX, scaleY, offset, _client->getEntityID());
		entity->render(_renderer->getSDLRenderer());             // Rendering all entities

		// if (!skip) entity->setOriginalPosition(Position{ pos.x + offset.x, pos.y + offset.y });
	}

	_renderer->present();

	inputThread.join();
	callbackThread.join();
	communicationThread.join();
	sideScrollingThread.join();
}

// Handles the logic for peers in peer to peer mode
void GameEngine::handlePeerToPeerMode(int64_t elapsedTime) {
	SDL_PumpEvents();
	_renderer->clear();
	float deltaTime = static_cast<float>(elapsedTime) * 1e-8f;

	std::thread inputThread([this]() {
		_inputManager->process();
		});

	std::thread callbackThread([this]() {
		_onCycle();
		});

	std::thread communicationThread([this]() {
		_peer->receiveUpdates();
		});

	std::set<Entity*> entitiesWithCollisions = _collisionSystem->run(_entities);
	_physicsSystem->runForGivenEntities(deltaTime, entitiesWithCollisions, _peer->getEntitiesToProcess());

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
}
	
// Handles the singleplayer game engine logic.
void GameEngine::handleSinglePlayerMode(int64_t elapsedTime) {
	_renderer->clear();

	float deltaTime = static_cast<float>(elapsedTime) * 1e-8f;

	std::thread inputThread([this]() {
		_inputManager->process();
	});

	std::thread callbackThread([this]() {
		_onCycle();
	});

	std::thread physicsThread([this, deltaTime]() {
		std::set<Entity*> entitiesWithCollisions = _collisionSystem->run(_entities);        // Running the collision system
		_physicsSystem->run(deltaTime, entitiesWithCollisions);
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
SideScroller* GameEngine::getSideScroller() { return _sideScroller; }
GameState GameEngine::getGameState() { return _gameState; }
PhysicsSystem* GameEngine::getPhysicsSystem() { return _physicsSystem; }
Client* GameEngine::getClient() { return _client; }
Peer* GameEngine::getPeer() { return _peer; }
int GameEngine::getServerRefreshRateMs() const { return _serverRefreshRateMs; }

// Setters
void GameEngine::setGameState(GameState state) { _gameState = state; }
void GameEngine::setServerRefreshRateMs(int rate) { _serverRefreshRateMs = rate; }

// Runs on each game cycle
void GameEngine::setOnCycle(const std::function<void()> &cb) {
	_onCycle = cb;
}

void GameEngine::pauseGame() {
	_gameState = GameState::PAUSED;
	_timeline->pause();
	_physicsSystem->pause();
	if (_mode == Mode::CLIENT) _client->setGameState(GameState::PAUSED);
}

void GameEngine::resumeGame() {
	_gameState = GameState::PLAY;
	_timeline->resume();
	_physicsSystem->resume();
	if (_mode == Mode::CLIENT) _client->setGameState(GameState::PLAY);
}

void GameEngine::setGameSpeed(double speed) {
	_timeline->setSpeed(speed);
}


