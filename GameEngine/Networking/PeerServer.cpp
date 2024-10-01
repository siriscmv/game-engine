#include "PeerServer.h"

#include <Client.h>
#include <iostream>
#include <Server.h>
#include <string>
#ifdef __APPLE__
#include <zmq.hpp>
#else
#include <ZMQ/zmq.hpp>
#endif

PeerServer::PeerServer(const std::vector<Entity*>& worldEntities, const std::vector<Entity*>& playerEntities) {
	_context = zmq::context_t(1);
	_publisher = zmq::socket_t(_context, zmq::socket_type::pub);
	_responder = zmq::socket_t(_context, zmq::socket_type::rep);
    _worldEntities = worldEntities;
    _playerEntities = playerEntities;
    _availablePlayerEntities = playerEntities;
    _allEntities = _worldEntities;
    _allEntities.insert(_allEntities.end(), _availablePlayerEntities.begin(), _availablePlayerEntities.end());
	_nextPeerID = 0;
}

PeerServer::~PeerServer() {
    _publisher.close();
    _responder.close();
}

void PeerServer::initialize(int repPort, int pubPort, int startingPeerID) {
    _responder.bind("tcp://*:" + std::to_string(repPort));
    _publisher.bind("tcp://*:" + std::to_string(pubPort));
    _nextPeerID = startingPeerID;

    std::cout << "PeerServer initialized." << std::endl;
    std::cout << "REP socket bound to port: " << repPort << std::endl;
    std::cout << "PUB socket bound to port: " << pubPort << std::endl;
}

void PeerServer::run() {
    zmq::message_t request;

    if (_responder.recv(request, zmq::recv_flags::dontwait)) {
        std::string clientRequest(static_cast<char*>(request.data()), request.size());

        if (clientRequest == "CONNECT") {
            int peerID = _nextPeerID++;
            std::ostringstream responseStream;

            if (!_availablePlayerEntities.empty()) {
                // Assign a player entity to the peer
                Entity* assignedEntity = _availablePlayerEntities.back();
                _availablePlayerEntities.pop_back();
                _peerEntityMap[peerID] = assignedEntity;

                // Determine if this peer is the host
                if (_hostPeerID == -1) {
                    _hostPeerID = peerID;
                }
                bool isHost = (peerID == _hostPeerID);

                // Build response: peerID|assignedEntityID|isHost|peerEntityMap|worldEntities|playerEntities
                responseStream << peerID << "|" << assignedEntity->getEntityID() << "|" << (isHost ? 1 : 0) << "|";

                // Include list of current peers and their assigned entities
                bool first = true;
                for (const auto& entry : _peerEntityMap) {
                    if (!first) {
                        responseStream << ";";
                    }
                    else {
                        first = false;
                    }
                    responseStream << entry.first << "," << entry.second->getEntityID();
                }
                responseStream << "|";

                // Serialize world entities
                first = true;
                for (const auto& entity : _worldEntities) {
                    if (!first) {
                        responseStream << "\n";
                    }
                    else {
                        first = false;
                    }
                    responseStream << Server::serializeEntity(*entity);
                }
                responseStream << "|";

                // Serialize all player entities (including assigned one)
                first = true;
                for (const auto& entity : _playerEntities) {
                    if (!first) {
                        responseStream << "\n";
                    }
                    else {
                        first = false;
                    }
                    responseStream << Server::serializeEntity(*entity);
                }

                std::string response = responseStream.str();

                // Send response to peer
                zmq::message_t reply(response.size());
                memcpy(reply.data(), response.c_str(), response.size());
                _responder.send(reply, zmq::send_flags::none);

                // Broadcast new peer to existing peers
                std::ostringstream broadcastStream;
                broadcastStream << "NEW_PEER|" << peerID << "|" << assignedEntity->getEntityID();
                std::string broadcastMessage = broadcastStream.str();
                zmq::message_t broadcast(broadcastMessage.size());
                memcpy(broadcast.data(), broadcastMessage.c_str(), broadcastMessage.size());
                _publisher.send(broadcast, zmq::send_flags::none);

                std::cout << "Peer connected. Assigned Peer ID: " << peerID << ", Entity ID: " << assignedEntity->getEntityID() << std::endl;
            }
            else {
                // Showing error message when a peer tries to join after all player entities are already assigned 
                std::string response = "ERROR|No available player entities";
                zmq::message_t reply(response.size());
                memcpy(reply.data(), response.c_str(), response.size());
                _responder.send(reply, zmq::send_flags::none);

                std::cout << "Peer connection attempted, but no available player entities." << std::endl;
            }
        }
        else {
            std::cerr << "Invalid request: " << clientRequest << std::endl;
            std::string errorResponse = "ERROR|Invalid request";
            zmq::message_t reply(errorResponse.size());
            memcpy(reply.data(), errorResponse.c_str(), errorResponse.size());
            _responder.send(reply, zmq::send_flags::none);
        }
    }
}

std::vector<Entity*> PeerServer::getEntities() const {
    return _allEntities;
}