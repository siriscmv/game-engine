#include "Server.h"
#include <iostream>
#include <thread>
#include <chrono>

Server::Server(const std::vector<Entity*>& worldEntities, const std::vector<Entity*>& spawnPoints) {
    _context = zmq::context_t(1); 
    _publisher = zmq::socket_t(_context, zmq::socket_type::pub);
    _subscriber = zmq::socket_t(_context, zmq::socket_type::sub);
    _responder = zmq::socket_t(_context, zmq::socket_type::rep);

    _worldEntities = worldEntities;
    _spawnPoints = spawnPoints;
    _allEntities = worldEntities;

    _nextClientID = 0;      
    
    _engine = new GameEngine("Server-side simulation", 0, 0, Mode::SERVER);
    _engine->initialize(_allEntities);

    setRefreshRate();
}

Server::~Server() {
    _publisher.close();
    _subscriber.close();
    _engine->shutdown();
}

// Initializes the server. Binds ports into pub-sub and req-rep models.
void Server::initialize(int pubPort, int subPort, int reqPort) {   

    _publisher.bind("tcp://*:" + std::to_string(pubPort));
    _subscriber.bind("tcp://*:" + std::to_string(subPort));
    _responder.bind("tcp://*:" + std::to_string(reqPort));

    _subscriber.set(zmq::sockopt::subscribe, "keypress");                        // Subscribing to 'keypress' topic

    printf("Server started and listening on the following ports:\n");
    printf("Publisher port: %d\n", pubPort);
    printf("Subscriber port: %d\n", subPort);
    printf("Request-Response port: %d\n", reqPort);
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
            updateClientEntities();
            std::this_thread::sleep_for(std::chrono::milliseconds(16));              // 60hz tick rate
        }
    });

    gameEngineThread.join();
    clientThread.join();
}

// Returns the initial world info to clients who request it
void Server::handleClientHandeshake() {
    zmq::message_t request;

    if (_responder.recv(request, zmq::recv_flags::dontwait)) {
        std::string clientRequest(static_cast<char*>(request.data()), request.size());
        if (clientRequest == "CONNECT") {
            int clientId = _nextClientID++;

            // Pick a random spawn point
            int randomIndex = rand() % _spawnPoints.size();
            Entity* spawnPoint = _spawnPoints[randomIndex];

            // Get the spawn point's position and size
            Position spawnPos = spawnPoint->getOriginalPosition();
            Size spawnSize = spawnPoint->getSize();

            // Generate a random position within the spawn point boundaries
            float playerX = spawnPos.x + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (spawnSize.width - 50)));
            float playerY = spawnPos.y + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (spawnSize.height - 50)));

            // Creating a player entity on that random position within the spawn point's boundaries
            Entity* playerEntity = new Entity(Position(playerX, playerY), Size(50, 50));
            playerEntity->setEntityID(_allEntities.size() + _spawnPoints.size() + 1);
            _engine->getPhysicsSystem()->applyPhysics(*playerEntity, 9.8f);
            _engine->getEntities().push_back(playerEntity);

            // Store the player entity in the client Map
            _clientMap[clientId] = playerEntity;
            _allEntities.push_back(playerEntity);                           

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
        }
    }
}


// Listens to the button press data from clients 
void Server::listenToClientMessages() {
    try {
        zmq::message_t request;
        if (_subscriber.recv(request, zmq::recv_flags::dontwait)) {
            std::string message(static_cast<char*>(request.data()), request.size());
        
            if (message.rfind("keypress|", 0) == 0) {
                message = message.substr(9);  
                int clientId = std::stoi(message.substr(0, 4));
                std::string buttonPress = message.substr(4);

                printf("Received input from Client %d: %s\n", clientId, buttonPress.c_str());
                processClientInput(clientId, buttonPress);
            }
        }
        else {
            /*printf("No input from clients, continuing server loop.\n");*/
        }
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

// Broadcasts entity updates to all clients
void Server::updateClientEntities() {
    std::string messagePrefix = "entity_update|";
    std::string allEntitiesData = messagePrefix;

    for (Entity* entity : _allEntities) {
        allEntitiesData += serializeEntity(*entity) + "\n";  
    }

    zmq::message_t message(allEntitiesData.size());
    memcpy(message.data(), allEntitiesData.c_str(), allEntitiesData.size());
    _publisher.send(message, zmq::send_flags::none);    
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

// Serializes an entity to a JSON string
std::string Server::serializeEntity(const Entity& entity) {
    json jsonEntity = {
        {"id", entity.getEntityID()},
        {"x", entity.getOriginalPosition().x},
        {"y", entity.getOriginalPosition().y},
        {"width", entity.getSize().width},
        {"height", entity.getSize().height},
        {"type", entityTypeToString(entity.getEntityType())},
        {"velocityX", entity.getVelocityX()},
        {"velocityY", entity.getVelocityY()},
        {"accelerationX", entity.getAccelerationX()},
        {"accelerationY", entity.getAccelerationY()}
    };
    return jsonEntity.dump(); 
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