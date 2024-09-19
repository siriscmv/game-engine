#pragma once

#include "Entity.h"
#include <ZMQ/zmq.hpp>
#include <vector>

class Client {
public:
    Client();
    ~Client();

    void initialize(int pubPort = 5556, int subPort = 5555, int reqPort = 5557);
    bool handshakeWithServer();
    void sendInputToServer(const std::string& buttonPress);
    void receiveUpdatesFromServer();

    Entity* deserializeEntity(const std::string& json);

    void setClientID(int id);
    std::vector<Entity*> getEntities() const;

private:
    zmq::context_t _context;
    zmq::socket_t _publisher;
    zmq::socket_t _subscriber;
    zmq::socket_t _requester;

    std::vector<Entity*> _entities;    

    int _clientID;

};
