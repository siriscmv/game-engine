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
    _requester = zmq::socket_t(_context, zmq::socket_type:: req);
}

Peer::~Peer() {
	_publisher.close();

    for (auto _subscriber: _subscribers) {
        _subscriber->close();
    }
}

void Peer::initialize(int serverPort) {
    _requester.connect("tcp://localhost:" + std::to_string(serverPort));
}

std::vector<std::string> split(const std::string& str, char delim) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;

    while (std::getline(ss, item, delim)) {
        result.push_back(item);
    }

    return result;
}

bool Peer::handshakeWithServer() {
    try {
        zmq::message_t request("CONNECT", 7);                                     // Request
        _requester.send(request, zmq::send_flags::none);

        zmq::message_t reply;                                                     // Response
        auto result = _requester.recv(reply, zmq::recv_flags::none);

        if (!result) {
            printf("Failed to receive response from server.\n");
            return false;
        }

        std::string response(static_cast<char*>(reply.data()), reply.size());

        std::vector<std::string> parts = split(response, *"|");

        int peerId = atoi(parts[0].c_str());
        printf("Self PeerId: %d\n", peerId);

        _peerID = peerId;

        _publisher.connect("tcp://localhost:" + std::to_string(peerId));

        for (int i = 1; i < parts.size(); i++) {
            auto socket = zmq::socket_t(_context, zmq::socket_type::sub);
            socket.connect("tcp://localhost:" + parts[i]);
            socket.set(zmq::sockopt::subscribe, "entity_update");
            _subscribers.push_back(&socket);
        }

        zmq::message_t req2("ENTITIES", 8);                                     // Request
        _requester.send(request, zmq::send_flags::none);

        zmq::message_t rep2;                                                     // Response
        auto res2 = _requester.recv(reply, zmq::recv_flags::none);

        if (!result) {
            printf("Failed to receive response from server.\n");
            return false;
        }

        std::string resp2(static_cast<char*>(rep2.data()), rep2.size());
        std::vector<std::string> parts2 = split(resp2, *"|");

        for (const auto& entityStr: parts2) {
            Entity* entity = Client::deserializeEntity(entityStr);
            _entities.push_back(entity);
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


void Peer::broadcastUpdates() {
    std::ostringstream messageStream;
    messageStream << "entity_update|" << _peerID << "|";

    for (const auto _entity: _entities) {
        if (std::to_string(_entity->getEntityID()).rfind(std::to_string(_peerID), 0) == 0) {
            messageStream << _entity->getEntityID() << "," << _entity->getPosition().x << "," << _entity->getPosition().y << "|";
        }
    }

    std::string message = messageStream.str();

    zmq::message_t zmqMessage(message.size());
    memcpy(zmqMessage.data(), message.c_str(), message.size());
    _publisher.send(zmqMessage, zmq::send_flags::none);
}

void Peer::receiveUpdates() {
    for (auto _subscriber: _subscribers) {
        zmq::message_t reply;

        if (_subscriber->recv(reply, zmq::recv_flags::dontwait)) {
            std::string received_msg(static_cast<char*>(reply.data()), reply.size());

            if (received_msg.rfind("new_peer|", 0) == 0) {
                int newPeerId = atoi((split(received_msg, *"|")[1]).c_str());
                auto socket = zmq::socket_t(_context, zmq::socket_type::sub);
                socket.connect("tcp://localhost:" + std::to_string(newPeerId));
                socket.set(zmq::sockopt::subscribe, "entity_update");
                _subscribers.push_back(&socket);
            } else if (received_msg.rfind("entity_update|", 0) == 0) {
                std::vector<std::string> parts = split(received_msg, *"|");

                for (int i = 2; i < parts.size(); i++) {
                    std::vector<std::string> entityParts = split(parts[i], *",");
                    auto predicate = [entityParts](const Entity* entity) { return entity->getEntityID() == atoi(entityParts[0].c_str()); };
                    auto it = std::find_if(_entities.begin(), _entities.end(), predicate);


                    if (it != _entities.end()) {
                        (*it)->setPosition(Position(std::stof(entityParts[1]), std::stof(entityParts[2])));
                    }
                }
            }
        }
    }
}



// Setters and getters
int Peer::getPeerId() const {return _peerID;}
void Peer::setPeerID(int id) { _peerID = id; }
std::vector<Entity*> Peer::getEntities() const { return _entities; }