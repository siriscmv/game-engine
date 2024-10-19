#include "Client.h"
#include <iostream>
#include <string>
#include <thread>
#ifdef __APPLE__
#include <zmq.hpp>
#else
#include <ZMQ/zmq.hpp>
#endif

Client::Client() {    
	_context = zmq::context_t(1);
	_publisher = zmq::socket_t(_context, zmq::socket_type::pub);
	_heartbeatPublisher = zmq::socket_t(_context, zmq::socket_type::pub);
    _entitySubscriber = zmq::socket_t(_context, zmq::socket_type::sub);
	_subscriber = zmq::socket_t(_context, zmq::socket_type::sub);
    _requester = zmq::socket_t(_context, zmq::socket_type:: req);
    _gameState = GameState::PLAY;
    setRefreshRate();

    _viewOffset = Position(0, 0);
}

Client::~Client() {
	_publisher.close();
    _heartbeatPublisher.close();
	_subscriber.close();
    _entitySubscriber.close();
}

// Initializes the client. Binds ports into pub-sub and req-rep models.
void Client::initialize(int pubPort, int entitySubPort, int reqPort, int heartbearPort, int subPort) {    

    _publisher.connect("tcp://localhost:" + std::to_string(pubPort));   
    _heartbeatPublisher.connect("tcp://localhost:" + std::to_string(heartbearPort));
    _entitySubscriber.connect("tcp://localhost:" + std::to_string(entitySubPort));
    _subscriber.connect("tcp://localhost:" + std::to_string(subPort));
    _requester.connect("tcp://localhost:" + std::to_string(reqPort));  
    _entitySubscriber.set(zmq::sockopt::subscribe, "entity_update");
    _subscriber.set(zmq::sockopt::subscribe, "");

    printf("Client initialized.\n");
    printf("Connected to server on ports:\n");
    printf("Publisher port: %d\n", pubPort);
    printf("Hearbeat port: %d\n", heartbearPort);
    printf("Entity updates subscriber port: %d\n", entitySubPort);
    printf("General subscriber port: %d\n", subPort);
    printf("Request-Response port: %d\n", reqPort);
}

// Retreives client ID and initial world information from server
bool Client::handshakeWithServer() {
    try {
        zmq::message_t request("CONNECT", 7);  // Request to connect
        _requester.send(request, zmq::send_flags::none);

        zmq::message_t reply;  // Response from server
        auto result = _requester.recv(reply, zmq::recv_flags::none);

        if (!result) {
            printf("Failed to receive response from server.\n");
            return false;
        }

        std::string response(static_cast<char*>(reply.data()), reply.size());        

        // Validate response format (expected format: "clientID|assignedEntityID|entityData")
        size_t firstSeparator = response.find("|");
        size_t secondSeparator = response.find("|", firstSeparator + 1);

        if (firstSeparator == std::string::npos || secondSeparator == std::string::npos) {
            printf("Invalid response format from server.\n");
            return false;
        }

        // Extract client ID and assigned entity ID
        setClientID(std::stoi(response.substr(0, firstSeparator)));
        _entityID = std::stoi(response.substr(firstSeparator + 1, secondSeparator - firstSeparator - 1));
        std::string entityData = response.substr(secondSeparator + 1);

        // Parse entity data and deserialize entities
        size_t pos = 0;
        while ((pos = entityData.find("\n")) != std::string::npos) {
            std::string serializedEntity = entityData.substr(0, pos);
            Entity* entity = deserializeEntity(serializedEntity);
            if (entity) {
                _entities.push_back(entity);
            }
            entityData.erase(0, pos + 1);
        }        

        printf("Successfully connected to server with Client ID: %d, Assigned Entity ID: %d\n", _clientID, _entityID);
        printf("Received %d entities from server.\n", static_cast<int>(_entities.size()));
        return true;
    }
    catch (const zmq::error_t& e) {
        std::cerr << "Connection error: " << e.what() << std::endl;
        return false;
    }
    catch (const std::exception& e) {
        std::cerr << "Handshake error: " << e.what() << std::endl;
        return false;
    }
}

// Sends heartbeat messages to the server
void Client::sendHeartbeatToServer() {    
    json heartbeatMessage = {
        {"type", "heartbeat"},
        {"clientId", _clientID}
    };
    
    std::string message = heartbeatMessage.dump();
   
    zmq::message_t zmqMessage(message.size());
    memcpy(zmqMessage.data(), message.c_str(), message.size());
    _heartbeatPublisher.send(zmqMessage, zmq::send_flags::none);
}


// Sends keypresses to the server
void Client::sendInputToServer(const std::string& buttonPress) {
    json keypressMessage = {
        {"type", "keypress"},
        {"clientId", _clientID},
        {"buttonPress", buttonPress}
    };
    
    std::string message = keypressMessage.dump();
    
    zmq::message_t zmqMessage(message.size());
    memcpy(zmqMessage.data(), message.c_str(), message.size());
    _publisher.send(zmqMessage, zmq::send_flags::none);

    printf("Sent input to server: %s\n", buttonPress.c_str());
}


// Receives entity updates from the server
void Client::receiveEntityUpdatesFromServer() {
    zmq::message_t update;

    if (_entitySubscriber.recv(update, zmq::recv_flags::dontwait)) {
        std::string allEntityUpdates(static_cast<char*>(update.data()), update.size());

        // Check for the "entity_update|" prefix and remove it
        if (allEntityUpdates.rfind("entity_update|", 0) == 0) {
            allEntityUpdates = allEntityUpdates.substr(14);

            size_t pos = 0;
            std::string delimiter = "\n";

            while ((pos = allEntityUpdates.find(delimiter)) != std::string::npos) {
                std::string serializedEntity = allEntityUpdates.substr(0, pos);
                Entity* updatedEntity = deserializeEntity(serializedEntity);

                for (Entity* entity : _entities) {
                    if (entity->getZoneType() == ZoneType::SIDESCROLL) continue;
                    if (_gameState == GameState::PAUSED && entity->getEntityID() == _entityID) continue;
                    if (entity->getEntityID() == updatedEntity->getEntityID()) {
                        entity->setOriginalPosition(updatedEntity->getOriginalPosition());
                        entity->setSize(updatedEntity->getSize());
                        break;
                    }
                }

                delete updatedEntity;
                allEntityUpdates.erase(0, pos + delimiter.length());
            }            
        }
    }
}

// Receives all other messages from the server apart from entity updates
void Client::receiveMessagesFromServer() {
    zmq::message_t update;

    if (_subscriber.recv(update, zmq::recv_flags::dontwait)) {
        std::string message(static_cast<char*>(update.data()), update.size());
        json jsonMessage = json::parse(message);

        // Handles client disconnect message
        if (jsonMessage["type"] == "disconnect") {
            int entityID = jsonMessage["entityID"];

            // Iterate through _entities and remove the one with the matching entityID
            for (auto it = _entities.begin(); it != _entities.end(); ++it) {
                if ((*it)->getEntityID() == entityID) {
                    delete* it;  
                    _entities.erase(it);  
                    printf("A player disconnected. Their player entity was removed.\n");
                    break;  
                }
            }
        }
        // Handles new connection message
        else if (jsonMessage["type"] == "new_connection") {
            std::string serializedEntity = jsonMessage["entity"];

            // Deserialize and add the new entity to the entities list
            Entity* newEntity = deserializeEntity(serializedEntity);
            if (newEntity && newEntity->getEntityID() != _entityID) {
                _entities.push_back(newEntity);
                printf("A new player has connected. Their player entity ID: %d\n", newEntity->getEntityID());
            }
        }
    }
}


// Converts entity type string into an enum variable
EntityType stringToEntityType(const std::string& str) {
    if (str == "DEFAULT") return EntityType::DEFAULT;
    else if (str == "FIXED") return EntityType::FIXED;
    else if (str == "ELASTIC") return EntityType::ELASTIC;
    else if (str == "GHOST") return EntityType::GHOST;
    else return EntityType::DEFAULT; 
}

// Converts zone type string into an enum variable
ZoneType stringToZoneType(const std::string& str) {
    if (str == "SPAWN") return ZoneType::SPAWN;
    else if (str == "DEATH") return ZoneType::DEATH;
    else if (str == "SIDESCROLL") return ZoneType::SIDESCROLL;
    else return ZoneType::NONE;  
}

// Deserializes entity info from a JSON string and creates an Entity object with it
Entity* Client::deserializeEntity(const std::string& jsonString) {
    try {
        // Parse the JSON string
        json jsonEntity = json::parse(jsonString);

        // Extract values
        int id = jsonEntity["id"];
        float x = jsonEntity["x"];
        float y = jsonEntity["y"];
        float width = jsonEntity["width"];
        float height = jsonEntity["height"];
        std::string typeStr = jsonEntity["type"];
        EntityType entityType = stringToEntityType(typeStr);
        std::string zoneTypeStr = jsonEntity["zoneType"];  
        ZoneType zoneType = stringToZoneType(zoneTypeStr); 

        float velocityX = jsonEntity["velocityX"];
        float velocityY = jsonEntity["velocityY"];
        float accelerationX = jsonEntity["accelerationX"];
        float accelerationY = jsonEntity["accelerationY"];

        // Create the entity
        Entity* entity = new Entity(Position(x, y), Size(width, height));
        entity->setEntityID(id);
        entity->setEntityType(entityType);
        entity->setZoneType(zoneType);
        entity->setVelocityX(velocityX);
        entity->setVelocityY(velocityY);
        entity->setAccelerationX(accelerationX);
        entity->setAccelerationY(accelerationY);

        return entity;
    }
    catch (const nlohmann::json::exception& e) {
        printf("Deserialization error: %s\n", e.what());
        return nullptr;
    }
}

// Setters and getters
void Client::setClientID(int id) { _clientID = id; }
void Client::setGameState(GameState gameState) { _gameState = gameState; }
std::vector<Entity*> Client::getEntities() const { return _entities; }
int Client::getClientID() { return _clientID; };
int Client::getEntityID() { return _entityID; };


void Client::setRefreshRate(RefreshRate rate) {
    _refreshRate = rate;
    _refreshRateMs = 1000 / static_cast<int>(rate);
}

RefreshRate Client::getRefreshRate() const { return _refreshRate; }
int Client::getRefreshRateMs() const { return _refreshRateMs; }

Position Client::getViewOffset() {
    return _viewOffset;
}

void Client::setViewOffset(Position viewOffset) {
    _viewOffset = viewOffset;
}

