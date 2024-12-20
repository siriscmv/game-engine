#include "Peer.h"

#include <Client.h>
#include <iostream>
#include <string>
#ifdef __APPLE__
#include <zmq.hpp>
#else
#include <ZMQ/zmq.hpp>
#endif

Peer::Peer() {
    _context = zmq::context_t(1);
    _publisher = zmq::socket_t(_context, zmq::socket_type::pub);
    _subscriber = zmq::socket_t(_context, zmq::socket_type::sub);
    _requester = zmq::socket_t(_context, zmq::socket_type::req);
    _peerID = -1;
    _entityID = -1;
    setRefreshRate();
}

Peer::~Peer() {
    _publisher.close();
    _subscriber.close();
    _requester.close();
}

void Peer::initialize(int reqPort, int serverPubPort) {
    _requester.connect("tcp://localhost:" + std::to_string(reqPort));
    _subscriber.connect("tcp://localhost:" + std::to_string(serverPubPort));
    _subscriber.set(zmq::sockopt::subscribe, "");

    std::cout << "Peer initialized." << std::endl;
    std::cout << "REQ socket connected to port: " << reqPort << std::endl;
    std::cout << "Subscribed to server's PUB socket on port: " << serverPubPort << std::endl;
}

std::vector<std::string> Peer::split(const std::string& str, char delim) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;

    while (std::getline(ss, item, delim)) {
        result.push_back(item);
    }

    return result;
}

// Initial handshake with the peer server. Gets the peer ID, info about entities, and other peers.
bool Peer::handshakeWithServer() {
    try {
        std::string connectRequest = "CONNECT";
        zmq::message_t request(connectRequest.size());
        memcpy(request.data(), connectRequest.c_str(), connectRequest.size());
        _requester.send(request, zmq::send_flags::none);

        zmq::message_t reply;
        auto result = _requester.recv(reply, zmq::recv_flags::none);

        if (!result) {
            std::cerr << "Failed to receive response from server." << std::endl;
            return false;
        }

        std::string response(static_cast<char*>(reply.data()), reply.size());
        std::vector<std::string> parts = split(response, '|');

        if (parts.size() < 4) {
            std::cerr << "Invalid response from server." << std::endl;
            return false;
        }

        // Get peer ID and assigned player entity ID
        _peerID = std::stoi(parts[0]);
        _entityID = std::stoi(parts[1]);
        _isHost = (std::stoi(parts[2]) == 1);
        std::cout << "Assigned Peer ID: " << _peerID << ", Entity ID: " << _entityID << ", Is Host: " << _isHost << std::endl;

        // Parse the peerEntityMap
        std::vector<std::string> peerEntityPairs = split(parts[3], ';');
        for (const auto& pairStr : peerEntityPairs) {
            if (pairStr.empty()) continue;
            std::vector<std::string> pair = split(pairStr, ',');
            if (pair.size() == 2) {
                int peerID = std::stoi(pair[0]);
                int entityID = std::stoi(pair[1]);
                _peerEntityMap[peerID] = entityID;
                if (peerID != _peerID) {
                    _knownPeers.push_back(peerID);
                }
            }
        }

        // Subscribe to existing peers' publishers. Peers' publish port is equal to their peer ID
        for (int peerID : _knownPeers) {
            int peerPubPort = peerID;                         
            _subscriber.connect("tcp://localhost:" + std::to_string(peerPubPort));
            std::cout << "Subscribed to peer ID: " << peerID << " on port: " << peerPubPort << std::endl;
        }

        // Initialize publisher socket.
        int pubPort = _peerID; 
        _publisher.bind("tcp://*:" + std::to_string(pubPort));
        std::cout << "PUB socket bound to port: " << pubPort << std::endl;

        // Deserialize world entities 
        std::string worldEntitiesData = parts[4];
        std::vector<std::string> worldEntityLines = split(worldEntitiesData, '\n');
        for (const auto& entityStr : worldEntityLines) {
            if (entityStr.empty()) continue;
            Entity* entity = Client::deserializeEntity(entityStr);
            if (entity) {
                _worldEntities.push_back(entity);
                _entities.push_back(entity);
            }
        }

        // Deserialize player entities
        std::string playerEntitiesData = parts[5];
        std::vector<std::string> playerEntityLines = split(playerEntitiesData, '\n');
        for (const auto& entityStr : playerEntityLines) {
            if (entityStr.empty()) continue;
            Entity* entity = Client::deserializeEntity(entityStr);
            if (entity) {
                _playerEntities.push_back(entity);
                _entities.push_back(entity);
            }
        }       

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

// Broadcasts player entity updates to other peers
void Peer::broadcastUpdates() {
    if (_isHost) {
        // Host peer broadcasts updates for all world entities
        for (auto entity : _worldEntities) {
            // Broadcast world entity update
            std::ostringstream messageStream;
            messageStream << "WORLD_UPDATE|" << entity->getEntityID() << "|" << entity->getOriginalPosition().x << "," << entity->getOriginalPosition().y;
            std::string message = messageStream.str();
            zmq::message_t zmqMessage(message.size());
            memcpy(zmqMessage.data(), message.c_str(), message.size());
            _publisher.send(zmqMessage, zmq::send_flags::none);
        }
    }

    // All peers broadcast their own player entity updates
    for (auto entity : _playerEntities) {
        if (entity->getEntityID() == _entityID) {
            std::ostringstream messageStream;
            messageStream << "PEER_UPDATE|" << _peerID << "|" << _entityID << "|" << entity->getOriginalPosition().x << "," << entity->getOriginalPosition().y;
            std::string message = messageStream.str();
            zmq::message_t zmqMessage(message.size());
            memcpy(zmqMessage.data(), message.c_str(), message.size());
            _publisher.send(zmqMessage, zmq::send_flags::none);
            break;
        }
    }
}

// Receives player entity updates from other peers
void Peer::receiveUpdates() {
    zmq::message_t update;

    while (_subscriber.recv(update, zmq::recv_flags::dontwait)) {
        std::string received_msg(static_cast<char*>(update.data()), update.size());

        if (received_msg.rfind("NEW_PEER|", 0) == 0) {
            // Handle new peer joining
            std::vector<std::string> parts = split(received_msg, '|');
            if (parts.size() != 3) continue;

            int newPeerID = std::stoi(parts[1]);
            int newEntityID = std::stoi(parts[2]);

            if (newPeerID != _peerID && _peerEntityMap.find(newPeerID) == _peerEntityMap.end()) {
                _peerEntityMap[newPeerID] = newEntityID;
                _knownPeers.push_back(newPeerID);

                // Subscribe to the new peer's publisher
                int newPeerPubPort = newPeerID;
                _subscriber.connect("tcp://localhost:" + std::to_string(newPeerPubPort));
                std::cout << "Subscribed to new peer ID: " << newPeerID << " on port: " << newPeerPubPort << std::endl;
            }

        }
        else if (received_msg.rfind("PEER_UPDATE|", 0) == 0) {
            // Handle peer updates
            std::vector<std::string> parts = split(received_msg, '|');
            if (parts.size() < 4) continue;

            int senderPeerID = std::stoi(parts[1]);
            int senderEntityID = std::stoi(parts[2]);

            if (senderPeerID == _peerID) continue;

            std::vector<std::string> positionParts = split(parts[3], ',');
            if (positionParts.size() != 2) continue;

            float posX = std::stof(positionParts[0]);
            float posY = std::stof(positionParts[1]);

            for (auto& entity : _playerEntities) {
                if (entity->getEntityID() == senderEntityID) {
                    entity->setOriginalPosition(Position(posX, posY));
                    break;
                }
            }
        }
        else if (received_msg.rfind("WORLD_UPDATE|", 0) == 0) {
            // Handle world entity updates from host
            std::vector<std::string> parts = split(received_msg, '|');
            if (parts.size() < 3) continue;

            int entityID = std::stoi(parts[1]);

            std::vector<std::string> positionParts = split(parts[2], ',');
            if (positionParts.size() != 2) continue;

            float posX = std::stof(positionParts[0]);
            float posY = std::stof(positionParts[1]);

            for (auto& entity : _worldEntities) {
                if (entity->getEntityID() == entityID) {
                    entity->setOriginalPosition(Position(posX, posY));
                    break;
                }
            }
        }
    }
}

// Returns the entity list that physics should be applied to, for this peer.
std::vector<Entity*> Peer::getEntitiesToProcess() const {
    std::vector<Entity*> entitiesToProcess;

    // Always process own player entity
    for (auto entity : _playerEntities) {
        if (entity->getEntityID() == _entityID) {
            entitiesToProcess.push_back(entity);
            break;
        }
    }

    // If host, also process world entities
    if (_isHost) {
        entitiesToProcess.insert(
            entitiesToProcess.end(),
            _worldEntities.begin(),
            _worldEntities.end()
        );
    }

    return entitiesToProcess;
}

// Setters and getters
int Peer::getPeerId() const {return _peerID;}
void Peer::setPeerID(int id) { _peerID = id; }
std::vector<Entity*> Peer::getEntities() const { return _entities; }
bool Peer::isHost() const { return _isHost; }
int Peer::getPlayerEntityID() const { return _entityID; }
std::vector<Entity*> Peer::getWorldEntities() const { return _worldEntities; }
std::vector<Entity*> Peer::getPlayerEntities() const { return _playerEntities; }
void Peer::setRefreshRate(RefreshRate rate) {
    _refreshRate = rate;
    _refreshRateMs = 1000 / static_cast<int>(rate);
}

RefreshRate Peer::getRefreshRate() const { return _refreshRate; }
int Peer::getRefreshRateMs() const { return _refreshRateMs; }
