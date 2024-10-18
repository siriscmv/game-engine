#include "Client.h"
#include <iostream>
#include <string>
#ifdef __APPLE__
#include <zmq.hpp>
#else
#include <ZMQ/zmq.hpp>
#endif

Client::Client() {    
	_context = zmq::context_t(1);
	_publisher = zmq::socket_t(_context, zmq::socket_type::pub);
	_subscriber = zmq::socket_t(_context, zmq::socket_type::sub);
    _requester = zmq::socket_t(_context, zmq::socket_type:: req);
    _gameState = GameState::PLAY;
    setRefreshRate();

    _viewOffset = Position(0, 0);
}

Client::~Client() {
	_publisher.close();
	_subscriber.close();
}

// Initializes the client. Binds ports into pub-sub and req-rep models.
void Client::initialize(int pubPort, int subPort, int reqPort) {    

    _publisher.connect("tcp://localhost:" + std::to_string(pubPort));   
    _subscriber.connect("tcp://localhost:" + std::to_string(subPort)); 
    _requester.connect("tcp://localhost:" + std::to_string(reqPort));  
    _subscriber.set(zmq::sockopt::subscribe, "entity_update");

    printf("Client initialized.\n");
    printf("Connected to server on ports:\n");
    printf("Publisher port: %d\n", pubPort);
    printf("Subscriber port: %d\n", subPort);
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

        // Check if the server is full
        if (response == "FULL") {
            printf("Server full, cannot connect\n");
            return false;
        }

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


// Sends keypresses to the server
void Client::sendInputToServer(const std::string& buttonPress) {
    std::ostringstream messageStream;
    messageStream << "keypress|" << std::setw(4) << std::setfill('0') << _clientID << buttonPress;   // Ensuring fixed length for client ID
    std::string message = messageStream.str();

    zmq::message_t zmqMessage(message.size());
    memcpy(zmqMessage.data(), message.c_str(), message.size());
    _publisher.send(zmqMessage, zmq::send_flags::none);

    printf("Sent input to server: %s\n", buttonPress.c_str());
}


// Receives entity updates from the server
void Client::receiveUpdatesFromServer() {
    zmq::message_t update;

    if (_subscriber.recv(update, zmq::recv_flags::dontwait)) {
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

            // printf("Received entity updates from server, continuing game loop\n");
        }
    }
    else {
        // printf("No updates from server, continuing game loop.\n");
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

// Deserializes entity info from a JSON string and creates and Entity object with it
Entity* Client::deserializeEntity(const std::string& json) {
    try {
        // Find positions of the fields
        size_t idPos = json.find("\"id\": ");
        size_t xPos = json.find("\"x\": ");
        size_t yPos = json.find("\"y\": ");
        size_t widthPos = json.find("\"width\": ");
        size_t heightPos = json.find("\"height\": ");
        size_t typePos = json.find("\"type\": \"");
        size_t velocityXPos = json.find("\"velocityX\": ");
        size_t velocityYPos = json.find("\"velocityY\": ");
        size_t accelerationXPos = json.find("\"accelerationX\": ");
        size_t accelerationYPos = json.find("\"accelerationY\": ");

        // Ensure all required fields exist
        if (idPos == std::string::npos || xPos == std::string::npos || yPos == std::string::npos ||
            widthPos == std::string::npos || heightPos == std::string::npos || typePos == std::string::npos ||
            velocityXPos == std::string::npos || velocityYPos == std::string::npos ||
            accelerationXPos == std::string::npos || accelerationYPos == std::string::npos) {
            printf("Error: Invalid JSON format for entity.\n");
            return nullptr;
        }

        // Extract values
        int id = std::stoi(json.substr(idPos + 6, json.find(",", idPos) - (idPos + 6)));
        float x = std::stof(json.substr(xPos + 5, json.find(",", xPos) - (xPos + 5)));
        float y = std::stof(json.substr(yPos + 5, json.find(",", yPos) - (yPos + 5)));
        float width = std::stof(json.substr(widthPos + 9, json.find(",", widthPos) - (widthPos + 9)));
        float height = std::stof(json.substr(heightPos + 10, json.find(",", heightPos) - (heightPos + 10)));

        // Extract entity type
        size_t typeValueStart = typePos + 9; // Length of "\"type\": \"" is 9
        size_t typeValueEnd = json.find("\"", typeValueStart);
        std::string typeStr = json.substr(typeValueStart, typeValueEnd - typeValueStart);
        EntityType entityType = stringToEntityType(typeStr);

        // Extract velocity and acceleration
        float velocityX = std::stof(json.substr(velocityXPos + 13, json.find(",", velocityXPos) - (velocityXPos + 13)));
        float velocityY = std::stof(json.substr(velocityYPos + 13, json.find(",", velocityYPos) - (velocityYPos + 13)));
        float accelerationX = std::stof(json.substr(accelerationXPos + 16, json.find(",", accelerationXPos) - (accelerationXPos + 16)));

        // For accelerationY (last field), find the closing brace
        size_t accelerationYEnd = json.find("}", accelerationYPos);
        float accelerationY = std::stof(json.substr(accelerationYPos + 16, accelerationYEnd - (accelerationYPos + 16)));

        // Create the entity
        Entity* entity = new Entity(Position(x, y), Size(width, height));
        entity->setEntityID(id);
        entity->setEntityType(entityType);
        entity->setVelocityX(velocityX);
        entity->setVelocityY(velocityY);
        entity->setAccelerationX(accelerationX);
        entity->setAccelerationY(accelerationY);

        // printf("Deserialized entity ID: %d, Position: (%f, %f), Type: %s\n", id, x, y, typeStr.c_str());

        return entity;
    }
    catch (const std::exception& e) {
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

