#include "GameEngine.h"
#include "TypedEventHandler.h"
#include "CollisionEvent.cpp"
#include "DeathEvent.cpp"
#include "EntityUpdateEvent.cpp"
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
	initializeCamera(windowWidth, windowHeight);
	_renderer = new Renderer();
	_gameState = GameState::PLAY;
	_inputManager = new InputManager(true);
	_physicsSystem = &PhysicsSystem::getInstance();
	_collisionSystem = &CollisionSystem::getInstance();
	_onCycle = []() {};
	_timeline = new Timeline();
	_eventManager = new EventManager(_timeline);
	_replaySystem = new ReplaySystem(_timeline);

	if (mode == Mode::CLIENT) _client = new Client();
	if (mode == Mode::PEER) _peer = new Peer();	
}

GameEngine::~GameEngine() {
	delete _renderer;
	delete _window;
	delete _inputManager;
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
	
	setUpEventHandlers();
	return true;
}

// Sets up event handlers 
void GameEngine::setUpEventHandlers() {	
	// Handker for collision events
	const EventHandler collisionHandler = TypedEventHandler<CollisionEvent>([this](const CollisionEvent* event) {
		Entity* entityA = event->getEntityA();
		Entity* entityB = event->getEntityB();

		_collisionSystem->handleCollision(entityA);
		_collisionSystem->handleCollision(entityB);
		
		});

	// Handler for death events 
	const EventHandler deathHandler = TypedEventHandler<DeathEvent>([this](const DeathEvent* event) {
		Entity* entity = event->getEntity();		

		// Set the entity's type back to default
		entity->setEntityType(EntityType::DEFAULT);
		entity->setAccelerationY(9.8f);		
		
		});

	const EventHandler entityUpdateHandler = TypedEventHandler<EntityUpdateEvent>([this](const EntityUpdateEvent* event) {
		const Entity* updatedEntity = event->getEntity();
		if (_replaySystem->isReplaying() && !event->isReplay()) {
			// Drop non-replay events if replay is in progress
			return;
		}

		for (Entity* entity : _entities) {
			if (entity->getZoneType() == ZoneType::SIDESCROLL) continue;
			if (_gameState == GameState::PAUSED && entity->getEntityID() == _client->getEntityID()) continue;

			if (entity->getEntityID() == updatedEntity->getEntityID()) {
				*entity = *updatedEntity;
				break;
			}
		}

		if (_replaySystem->isRecording()) {			
			_replaySystem->handler(event);
			
		}
	});

	const EventHandler replayHandler = TypedEventHandler<ReplayEvent>([this](const ReplayEvent *event) {
		const Entity* updatedEntity = event->getEntity();

		for (Entity* entity : _entities) {
			if (entity->getZoneType() == ZoneType::SIDESCROLL) continue;
			if (_gameState == GameState::PAUSED && entity->getEntityID() == _client->getEntityID()) continue;

			if (entity->getEntityID() == updatedEntity->getEntityID()) {
				*entity = *updatedEntity;
				break;
			}
		}
	});

	// Register the handler with the event manager
	_eventManager->registerHandler(EventType::Collision, collisionHandler);
	_eventManager->registerHandler(EventType::Death, deathHandler);
	_eventManager->registerHandler(EventType::EntityUpdate, entityUpdateHandler);
	_eventManager->registerHandler(EventType::Replay, replayHandler);
}

// Handles player entity collisions with death zones
void GameEngine::handleDeathZones() {
	for (Entity* entity : _entities) {
		if (entity->getZoneType() == ZoneType::DEATH) {
			// Loop through all players in the client map
			for (const auto& [clientId, playerEntity] : *_clientMap) {
				if (_collisionSystem->hasCollisionRaw(entity, playerEntity)) {

					// Find all spawn points
					std::vector<Entity*> spawnPoints;
					for (Entity* spawnEntity : _entities) {
						if (spawnEntity->getZoneType() == ZoneType::SPAWN) {
							spawnPoints.push_back(spawnEntity);
						}
					}

					// Error if no spawn points are found
					if (spawnPoints.empty()) {
						throw std::runtime_error("No spawn points found for teleportation!");
					}

					// Choose a random spawn point and calculate its center position
					int randomIndex = rand() % spawnPoints.size();
					Entity* spawnPoint = spawnPoints[randomIndex];
					Position spawnPos = spawnPoint->getOriginalPosition();
					Size spawnSize = spawnPoint->getSize();

					// Generate a random position within the spawn point boundaries
					float playerX = spawnPos.x + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (spawnSize.width - 50)));
					float playerY = spawnPos.y + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (spawnSize.height - 50)));
					Position newPosition(playerX, playerY);					

					playerEntity->setOriginalPosition(newPosition);
					playerEntity->setVelocityX(0);
					playerEntity->setVelocityY(0);
					playerEntity->setAccelerationY(0);
					playerEntity->setEntityType(EntityType::GHOST);

					// Raise a death event with delay to respawn the player
					_eventManager->raiseEventWithDelay(new DeathEvent(playerEntity, newPosition), entity->getEventDelay());
				}
			}
		}
	}
}


// Initializes the camera. Sets position and size.
void GameEngine::initializeCamera(int width, int height) {
	_camera.x = 0;  
	_camera.y = 0;
	_camera.size.width = width;  
	_camera.originalSize.width = width;  
	_camera.size.height = height;  
	_camera.originalSize.height = height;  
}

// Resizes the camera based on the scale factors passed in. 
void GameEngine::resizeCamera(float scaleX, float scaleY) {	
	_camera.size.width = static_cast<int>(_camera.originalSize.width * scaleX);
	_camera.size.height = static_cast<int>(_camera.originalSize.height * scaleY);
}


// Moves the camera's position to the passed in coordinates
void GameEngine::moveCamera(int newX, int newY) {
	_camera.x = newX;
	_camera.y = newY;
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

	_eventManager->process();
	_onCycle();
	handleDeathZones();                                                                                // Handling death zone collisions
	std::set<Entity*> entitiesWithCollisions = _collisionSystem->run(_entities, _eventManager);        // Running the collision system	                                                                                

	float deltaTime = static_cast<float>(elapsedTime) * 1e-8f;	
 	_physicsSystem->run(deltaTime, entitiesWithCollisions);	
}

// Handles the client's game engine logic in server-client multiplayer
void GameEngine::handleClientMode(int64_t elapsedTime) {
	SDL_PumpEvents(); // Force an event queue update
	_renderer->clear();

	_entities = _client->getEntities();

	std::thread heartbeatThread([this]() {
		_client->sendHeartbeatToServer();
		});

	std::thread eventThread([this]() {
		_eventManager->process();
		_inputManager->process(_eventManager);		
		_client->receiveEntityUpdatesFromServer(_eventManager);
		_client->receiveMessagesFromServer();
		});

	std::thread callbackThread([this]() {
		_onCycle();
		});

	auto [scaleX, scaleY] = _window->getScaleFactors();
	resizeCamera(scaleX, scaleY);

	// Rendering all entities
	for (Entity* entity : _entities) {
		entity->applyScaling(scaleX, scaleY);		

		// Only render if within the viewport and not ghost entities
		if (entity->isWithinViewPort(_camera) && entity->getEntityType() != EntityType::GHOST) {
			entity->render(_renderer->getSDLRenderer(), _camera);  
		}
	}

	_renderer->present();

	eventThread.join();
	callbackThread.join();	
	heartbeatThread.join();	
}

// Handles the logic for peers in peer to peer mode
void GameEngine::handlePeerToPeerMode(int64_t elapsedTime) {
	SDL_PumpEvents();
	_renderer->clear();
	float deltaTime = static_cast<float>(elapsedTime) * 1e-8f;

	std::thread inputThread([this]() {
		_inputManager->process(_eventManager);
		});

	std::thread callbackThread([this]() {
		_onCycle();
		});

	std::thread communicationThread([this]() {
		_peer->receiveUpdates();
		});

	std::set<Entity*> entitiesWithCollisions = _collisionSystem->run(_entities, _eventManager);
	_physicsSystem->runForGivenEntities(deltaTime, entitiesWithCollisions, _peer->getEntitiesToProcess());

	auto [scaleX, scaleY] = _window->getScaleFactors();

	// Rendering all entities
	for (Entity* entity : _entities) {
		entity->applyScaling(scaleX, scaleY);		
		entity->render(_renderer->getSDLRenderer(), _camera);
	}

	_renderer->present();

	_peer->broadcastUpdates();

	inputThread.join();
	callbackThread.join();
	communicationThread.join();	
}
	
// Handles the singleplayer game engine logic.
void GameEngine::handleSinglePlayerMode(int64_t elapsedTime) {
	SDL_PumpEvents(); // Force an event queue update
	_renderer->clear();

	float deltaTime = static_cast<float>(elapsedTime) * 1e-8f;

	std::thread inputThread([this]() {
		_inputManager->process(_eventManager);
	});

	std::thread callbackThread([this]() {
		_onCycle();
	});

	std::thread physicsThread([this, deltaTime]() {
		if (!_runPhysics) return;

		std::set<Entity*> entitiesWithCollisions = _collisionSystem->run(_entities, _eventManager);        // Running the collision system
		_physicsSystem->run(deltaTime, entitiesWithCollisions);
	});

	std::thread eventThread([this]() {
		_eventManager->process();
		});

	auto [scaleX, scaleY] = _window->getScaleFactors();

	// Rendering all entities
	for (Entity* entity : _entities) {
		entity->applyScaling(scaleX, scaleY);		
		entity->render(_renderer->getSDLRenderer(), _camera);              // Rendering all entities
	}

	_renderer->present();

	inputThread.join();
	callbackThread.join();
	physicsThread.join();
	eventThread.join();
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
EventManager* GameEngine::getEventManager() { return _eventManager; }
GameState GameEngine::getGameState() { return _gameState; }
PhysicsSystem* GameEngine::getPhysicsSystem() { return _physicsSystem; }
CollisionSystem* GameEngine::getCollisionSystem() { return _collisionSystem; }
Window* GameEngine::getWindow() { return _window; }
Client* GameEngine::getClient() { return _client; }
Peer* GameEngine::getPeer() { return _peer; }
int GameEngine::getServerRefreshRateMs() const { return _serverRefreshRateMs; }
std::vector<Entity*>& GameEngine::getEntities() { return _entities; }
Camera& GameEngine::getCamera() { return _camera; }
ReplaySystem* GameEngine::getReplaySystem() const { return _replaySystem; }


// Setters
void GameEngine::setGameState(GameState state) { _gameState = state; }
void GameEngine::setServerRefreshRateMs(int rate) { _serverRefreshRateMs = rate; }
void GameEngine::setClientMap(std::map<int, Entity*>& clientMap) { _clientMap = &clientMap; }

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

void GameEngine::setEntities(const std::vector<std::shared_ptr<Entity>> &entities)
{
	if (_mode != Mode::SINGLE_PLAYER) {
		throw std::runtime_error("Cannot set entities in multiplayer mode");
	}

	_entities.clear();
	_entityOwners = entities;
	_entities.reserve(entities.size());

	for (const auto &entity : entities) {
		_entities.push_back(entity.get());
	}
}

void GameEngine::enablePhysics() {
	_runPhysics = true;
}

void GameEngine::disablePhysics() {
	_runPhysics = false;
}
