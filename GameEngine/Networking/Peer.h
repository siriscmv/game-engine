#pragma once

#include "Entity.h"
#include <vector>
#ifdef __APPLE__
#include <zmq.hpp>
#else
#include <ZMQ/zmq.hpp>
#endif

class Peer {
public:
    Peer();
    ~Peer();

    void initialize(int serverPort = 10000);
    bool handshakeWithServer();
    void receiveUpdates();

    int getPeerId() const;

    void broadcastUpdates();

    void setPeerID(int id);
    std::vector<Entity*> getEntities() const;

private:
    zmq::context_t _context;
    zmq::socket_t _publisher;
    std::vector<zmq::socket_t*> _subscribers;
    zmq::socket_t _requester;

    std::vector<Entity*> _entities;

    int _peerID;
};
