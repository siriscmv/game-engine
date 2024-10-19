#include "Server.h"
#include <iostream>
#include <thread>
#include <chrono>

Server::Server(const std::vector<Entity*>& entities) {
    _context = zmq::context_t(1); 
    _entityPublisher = zmq::socket_t(_context, zmq::socket_type::pub);
    _publisher = zmq::socket_t(_context, zmq::socket_type::pub);
    _subscriber = zmq::socket_t(_context, zmq::socket_type::sub);
    _heartbeatSubscriber = zmq::socket_t(_context, zmq::socket_type::sub);
    _responder = zmq::socket_t(_context, zmq::socket_type::rep);
    
    _allEntities = entities;

    _nextClientID = 0;
    _nextEntityID = entities.size();
    
    _engine = new GameEngine("Server-side simulation", 0, 0, Mode::SERVER);
    _engine->initialize(_allEntities);
    _engine->setClientMap(_clientMap);

    setRefreshRate();
}

Server::~Server() {
    _entityPublisher.close();
    _subscriber.close();
    _heartbeatSubscriber.close();
    _publisher.close();
    _engine->shutdown();
}

// Initializes the server. Binds ports into pub-sub and req-rep models.
void Server::initialize(int entityPubPort, int subPort, int reqPort, int heartbeatPort, int pubPort) {   

    _entityPublisher.bind("tcp://*:" + std::to_string(entityPubPort));
    _publisher.bind("tcp://*:" + std::to_string(pubPort));
    _subscriber.bind("tcp://*:" + std::to_string(subPort));
    _heartbeatSubscriber.bind("tcp://*:" + std::to_string(heartbeatPort));
    _responder.bind("tcp://*:" + std::to_string(reqPort));

    _subscriber.set(zmq::sockopt::subscribe, "");
    _heartbeatSubscriber.set(zmq::sockopt::subscribe, "");

    printf("Server started and listening on the following ports:\n");
    printf("Entity updates publisher port: %d\n", entityPubPort);
    printf("General publisher port: %d\n", pubPort);
    printf("Subscriber port: %d\n", subPort);
    printf("Request-Response port: %d\n", reqPort);
    printf("Heartbeat port: %d\n", heartbeatPort);
}

// Server loop. Receives updates about keyboard inputs from clients. Sends updates 
// about the game world to clients.
void Server::run() {
    std::thread gameEngineThread([this]() {
        try {
            _engine->run();  
        }
        catch (const std::exception& e) {
            std::cerr << "Game engine encountered an error: " << e.what() << std::endl;
            std::terminate();  
        }
        });

    std::thread clientThread([this]() {
        while (true) {
            handleClientHandeshake();
            listenToClientMessages();
            monitorHeartbeats();
            updateClientEntities();
            std::this_thread::sleep_for(std::chrono::milliseconds(16));              // 60hz tick rate
        }
    });

    std::thread heartbeatThread([this]() {
        while (true) {
            listenToHeartbeatMessages();              
        }
        });

    gameEngineThread.join();
    clientThread.join();
    heartbeatThread.join();
}

// Returns the initial world info to clients who request it
void Server::handleClientHandeshake() {
    zmq::message_t request;

    if (_responder.recv(request, zmq::recv_flags::dontwait)) {
        std::string clientRequest(static_cast<char*>(request.data()), request.size());
        if (clientRequest == "CONNECT") {
            int clientId = _nextClientID++;

            // Collect all spawn points from the _allEntities list
            std::vector<Entity*> spawnPoints;
            for (Entity* entity : _allEntities) {
                if (entity->getZoneType() == ZoneType::SPAWN) {
                    spawnPoints.push_back(entity);
                }
            }

            // Throw an error if no spawn points are found
            if (spawnPoints.empty()) {
                throw std::runtime_error("No spawn points found in the world!");
            }

            // Pick a random spawn point
            int randomIndex = rand() % spawnPoints.size();
            Entity* spawnPoint = spawnPoints[randomIndex];

            // Get the spawn point's position and size
            Position spawnPos = spawnPoint->getOriginalPosition();
            Size spawnSize = spawnPoint->getSize();

            // Generate a random position within the spawn point boundaries
            float playerX = spawnPos.x + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (spawnSize.width - 50)));
            float playerY = spawnPos.y + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (spawnSize.height - 50)));

            // Creating a player entity on that random position within the spawn point's boundaries
            Entity* playerEntity = new Entity(Position(playerX, playerY), Size(50, 50));
            playerEntity->setEntityID(_nextEntityID++);
            playerEntity->setAccelerationY(9.8f);

            // Pushing the player entity into entity lists in the game engine and physics system
            _engine->getEntities().push_back(playerEntity);
            _engine->getPhysicsSystem()->getEntities().push_back(playerEntity);

            // Store the player entity in the client Map
            _clientMap[clientId] = playerEntity;
            _allEntities.push_back(playerEntity);

            // Initialize last heartbeat time
            _lastHeartbeatMap[clientId] = std::chrono::steady_clock::now();

            printf("Client connected with ID: %d, created Player Entity ID: %d at Spawn Point (%f, %f)\n",
                clientId, playerEntity->getEntityID(), spawnPos.x, spawnPos.y);

            // Create response with client ID, assigned entity ID, and all entity data
            std::string response = std::to_string(clientId) + "|" + std::to_string(playerEntity->getEntityID()) + "|";
            for (const auto& entity : _allEntities) {
                response += serializeEntity(*entity) + "\n";
            }

            zmq::message_t reply(response.size());
            memcpy(reply.data(), response.c_str(), response.size());
            _responder.send(reply, zmq::send_flags::none);

            // Notify all other clients about this new connection
            broadcastNewConnection(playerEntity);
        }
    }
}

// Moniors heartbeats of the clients to detect disconnects. Delegates to
// 'handleClientDisconnect' method upon detecting a disconnect.
void Server::monitorHeartbeats() {    
    auto now = std::chrono::steady_clock::now();

    std::vector<int> disconnectedClients;

    for (const auto& [clientId, lastHeartbeat] : _lastHeartbeatMap) {
        auto durationSinceLastHeartbeat = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastHeartbeat);
        
        if (durationSinceLastHeartbeat > _heartbeatTimeout) {
            disconnectedClients.push_back(clientId);
        }
    }

    for (int clientId : disconnectedClients) {
        handleClientDisconnect(clientId);
    }    
}

// Disconnect a client from the server and cleans up their player entity object 
void Server::handleClientDisconnect(int clientId) {
    printf("Client with ID: %d disconnected.\n", clientId);

    // Remove player entity from the game engine, physics system, and all entities list
    Entity* playerEntity = _clientMap[clientId];

    auto& gameEntities = _engine->getEntities();
    gameEntities.erase(std::remove(gameEntities.begin(), gameEntities.end(), playerEntity), gameEntities.end());
    
    auto& physicsEntities = _engine->getPhysicsSystem()->getEntities();
    physicsEntities.erase(std::remove(physicsEntities.begin(), physicsEntities.end(), playerEntity), physicsEntities.end());
    
    _allEntities.erase(std::remove(_allEntities.begin(), _allEntities.end(), playerEntity), _allEntities.end());

    // Remove from the client map and heartbeat map
    _clientMap.erase(clientId);
    _lastHeartbeatMap.erase(clientId);

    // Inform all clients about the disconnection
    broadcastDisconnect(playerEntity->getEntityID());

    delete playerEntity;  
}

// Broadcasts the newly connected player's entity details to already connected clients
void Server::broadcastNewConnection(Entity* playerEntity) {
    json newConnectionMessage = {
        {"type", "new_connection"},
        {"entity", serializeEntity(*playerEntity)}
    };

    std::string message = newConnectionMessage.dump();
    zmq::message_t zmqMessage(message.size());
    memcpy(zmqMessage.data(), message.c_str(), message.size());

    _publisher.send(zmqMessage, zmq::send_flags::none);
}

// Broadcasts a disconnect message to all clients
void Server::broadcastDisconnect(int playerEntityID) {
    json disconnectMessage = {
        {"type", "disconnect"},
        {"entityID", playerEntityID}
    };

    std::string message = disconnectMessage.dump();
    zmq::message_t zmqMessage(message.size());
    memcpy(zmqMessage.data(), message.c_str(), message.size());

    _publisher.send(zmqMessage, zmq::send_flags::none);  
}

// Listens to heartbeat messages from clients
void Server::listenToHeartbeatMessages() {
    try {
        zmq::message_t request;
        if (_heartbeatSubscriber.recv(request, zmq::recv_flags::dontwait)) {
            std::string message(static_cast<char*>(request.data()), request.size());
            json jsonMessage = json::parse(message);
            
            if (jsonMessage["type"] == "heartbeat") {
                int clientId = jsonMessage["clientId"];
                _lastHeartbeatMap[clientId] = std::chrono::steady_clock::now();                 
            }
        }
    }
    catch (const nlohmann::json::exception& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error processing heartbeat: " << e.what() << std::endl;
    }
}


// Listens to the button press data from clients 
void Server::listenToClientMessages() {
    try {
        zmq::message_t request;
        if (_subscriber.recv(request, zmq::recv_flags::dontwait)) {
            std::string message(static_cast<char*>(request.data()), request.size());

            json jsonMessage = json::parse(message);

            // Extract the message type and client ID
            std::string messageType = jsonMessage["type"];            
            int clientId = jsonMessage["clientId"];

            // Handle keypress messages
            if (messageType == "keypress") {
                std::string buttonPress = jsonMessage["buttonPress"];
                printf("Received input from Client %d: %s\n", clientId, buttonPress.c_str());
                processClientInput(clientId, buttonPress);
            }            
        }
    }
    catch (const nlohmann::json::exception& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error processing input: " << e.what() << std::endl;
    }
}


// Processes keypress from client and updates corresponding player entity velocity
void Server::processClientInput(int clientId, const std::string& buttonPress) {
    if (_clientMap.find(clientId) != _clientMap.end()) {
        Entity* playerEntity = _clientMap[clientId];
        
        if (buttonPress == "left") {
            playerEntity->setVelocityX(-50.0f);
        }
        else if (buttonPress == "right") {
            playerEntity->setVelocityX(50.0f);
        }
        else if (buttonPress == "up") {
            playerEntity->setVelocityY(-50.0f);
        }
        else if (buttonPress == "down") {
            playerEntity->setVelocityY(50.0f);
        }
    }
}

// Converts entity type into a string
std::string entityTypeToString(EntityType type) {
    switch (type) {
        case EntityType::DEFAULT: return "DEFAULT";
        case EntityType::FIXED: return "FIXED";
        case EntityType::ELASTIC: return "ELASTIC";
        case EntityType::GHOST: return "GHOST";
        default: return "DEFAULT";
    }
}

// Converts zone type into a string
std::string zoneTypeToString(ZoneType type) {
    switch (type) {
    case ZoneType::NONE: return "NONE";
    case ZoneType::SPAWN: return "SPAWN";
    case ZoneType::DEATH: return "DEATH";
    case ZoneType::SIDESCROLL: return "SIDESCROLL";
    default: return "NONE";
    }
}

std::string entityToString(const Entity& entity) {
    std::ostringstream oss;

    oss << "id:" << entity.getEntityID() << '|'
        << "x:" << entity.getOriginalPosition().x << '|'
        << "y:" << entity.getOriginalPosition().y << '|'
        << "width:" << entity.getSize().width << '|'
        << "height:" << entity.getSize().height << '|'
        << "type:" << entityTypeToString(entity.getEntityType()) << '|'
        << "velocityX:" << entity.getVelocityX() << '|'
        << "velocityY:" << entity.getVelocityY() << '|'
        << "accelerationX:" << entity.getAccelerationX() << '|'
        << "accelerationY:" << entity.getAccelerationY() << '|'
        << "cr:" << static_cast<int>(entity.getColor().r) << '|'
        << "cg:" << static_cast<int>(entity.getColor().g) << '|'
        << "cb:" << static_cast<int>(entity.getColor().b) << '|'
        << "ca:" << static_cast<int>(entity.getColor().a);

    return oss.str();
}

json entityToJson(const Entity& entity) {
    json jsonEntity = {
        {"id", entity.getEntityID()},
        {"x", entity.getOriginalPosition().x},
        {"y", entity.getOriginalPosition().y},
        {"width", entity.getSize().width},
        {"height", entity.getSize().height},
        {"type", entityTypeToString(entity.getEntityType())},
        {"zoneType", zoneTypeToString(entity.getZoneType())},
        {"velocityX", entity.getVelocityX()},
        {"velocityY", entity.getVelocityY()},
        {"accelerationX", entity.getAccelerationX()},
        {"accelerationY", entity.getAccelerationY()},
        {"cr", static_cast<int>(entity.getColor().r)},
        {"cg", static_cast<int>(entity.getColor().g)},
        {"cb", static_cast<int>(entity.getColor().b)},
        {"ca", static_cast<int>(entity.getColor().a)},
    };

    return jsonEntity;
}

constexpr bool useJSON = false;

// Broadcasts entity updates to all clients
void Server::updateClientEntities() {
    std::string allEntitiesData;

    if (useJSON) {
        json updateMessage = {
            {"type", "entity_update"},
            {"entities", json::array()}
        };

        for (const Entity* entity : _allEntities) {
            updateMessage["entities"].push_back(entityToJson(*entity));
        }

        allEntitiesData = updateMessage.dump();
    } else {
        std::ostringstream oss;
        oss << "entity_update|";
        for (const Entity* entity : _allEntities) {
            oss << "||" << entityToString(*entity);
        }
        allEntitiesData = oss.str();
    }

    zmq::message_t message(allEntitiesData.size());
    memcpy(message.data(), allEntitiesData.c_str(), allEntitiesData.size());
    _entityPublisher.send(message, zmq::send_flags::none);
}

// Serializes an entity to a JSON string
std::string Server::serializeEntity(const Entity& entity) {
    return entityToJson(entity).dump();
}

// Setter and getters
void Server::setRefreshRate(RefreshRate rate) {
    _refreshRate = rate;
    _refreshRateMs = 1000 / static_cast<int>(rate);
    _engine->setServerRefreshRateMs(_refreshRateMs);
}

// Changes the game simulation speed
void Server::setSimulationSpeed(double speed) {
    _engine->setGameSpeed(speed);
}

RefreshRate Server::getRefreshRate() const { return _refreshRate; }
int Server::getRefreshRateMs() const { return _refreshRateMs; }
GameEngine* Server::getGameEngine() const { return _engine; }

// Sets the heartbeat timeout value 
void Server::setHeartBeatTimeout(int milliseconds) {
    _heartbeatTimeout = std::chrono::milliseconds(milliseconds);
}