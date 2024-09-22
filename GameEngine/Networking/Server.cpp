#include "Server.h"
#include <iostream>
#include <thread>
#include <chrono>

Server::Server(const std::vector<Entity*>& worldEntities, const std::vector<Entity*>& playerEntities) {
    _context = zmq::context_t(1); 
    _publisher = zmq::socket_t(_context, zmq::socket_type::pub);
    _subscriber = zmq::socket_t(_context, zmq::socket_type::sub);
    _responder = zmq::socket_t(_context, zmq::socket_type::rep);


    _worldEntities = worldEntities;
    _playerEntities = playerEntities;
    _availablePlayerEntities = playerEntities;

    _nextClientID = 0;

    _allEntities = worldEntities;
    _allEntities.insert(_allEntities.end(), playerEntities.begin(), playerEntities.end());
    
    _engine = new GameEngine("Server-side simulation", 0, 0, Mode::SERVER);
    _engine->initialize(_allEntities);
}

Server::~Server() {
    _publisher.close();
    _subscriber.close();
    _engine->shutdown();
}

// Initializes the server. Binds ports into pub-sub and req-rep models.
void Server::initialize(int pubPort, int subPort, int reqPort) {

    _publisher.set(zmq::sockopt::sndhwm, 1000);  
    _subscriber.set(zmq::sockopt::rcvhwm, 1000); 

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

            // Assign a player entity and respond with client ID + entity info
            if (!_availablePlayerEntities.empty()) {
                Entity* assignedEntity = _availablePlayerEntities.back();
                _availablePlayerEntities.pop_back();
                _clientMap[clientId] = assignedEntity;

                printf("Client connected with ID: %d, assigned Entity ID: %d\n", clientId, assignedEntity->getEntityID());

                std::string response = std::to_string(clientId) + "|";
                for (const auto& entity : _allEntities) {
                    response += serializeEntity(*entity) + "\n";
                }

                zmq::message_t reply(response.size());
                memcpy(reply.data(), response.c_str(), response.size());
                _responder.send(reply, zmq::send_flags::none);
            }
            // If server is full respond with the message 'FULL' 
            else {
                std::string response = "FULL";
                zmq::message_t reply(response.size());
                memcpy(reply.data(), response.c_str(), response.size());
                _responder.send(reply, zmq::send_flags::none);

                printf("Client connection attempted, but server is full.\n");
            }
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

// Serializes an entity to a JSON string
std::string Server::serializeEntity(const Entity& entity) {
    std::ostringstream json;
    json << "{";
    json << "\"id\": " << entity.getEntityID() << ",";
    json << "\"x\": " << entity.getOriginalPosition().x << ",";
    json << "\"y\": " << entity.getOriginalPosition().y << ",";
    json << "\"width\": " << entity.getSize().width << ",";
    json << "\"height\": " << entity.getSize().height;
    json << "}";
    return json.str();
}

// Getters
GameEngine* Server::getGameEngine() const { return _engine; }