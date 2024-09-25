#pragma once

#include "Entity.h"
#include <vector>
#ifdef __APPLE__
#include <zmq.hpp>
#else
#include <ZMQ/zmq.hpp>
#endif

class PeerServer {
public:
    PeerServer();
    ~PeerServer();

    void initialize();
    void run();

    std::vector<Entity*> getEntities() const;

private:
    zmq::context_t _context;
    zmq::socket_t _publisher;
    zmq::socket_t _responder;
    std::vector<Entity*> _entities;
};
