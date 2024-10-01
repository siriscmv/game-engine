#pragma once

#include "Entity.h"
#include <vector>
#include <map>
#ifdef __APPLE__
#include <zmq.hpp>
#else
#include <ZMQ/zmq.hpp>
#endif

class PeerServer {
public:
    PeerServer(const std::vector<Entity*>& worldEntities, const std::vector<Entity*>& playerEntities);
    ~PeerServer();

    void initialize(int repPort = 5555, int pubPort = 5556, int startingPeerID = 1000);
    void run();
    std::vector<Entity*> getEntities() const;

private:
    zmq::context_t _context;
    zmq::socket_t _publisher;                              // PUB socket for broadcasting new peer connections to peers
    zmq::socket_t _responder;                              // REP socket for handling initial connection requests from peers
    std::vector<Entity*> _worldEntities;
    std::vector<Entity*> _playerEntities;
    std::vector<Entity*> _availablePlayerEntities;
    std::vector<Entity*> _allEntities;
    std::map<int, Entity*> _peerEntityMap;                 // Map of peerID to assigned player entity

    int _nextPeerID;
    int _hostPeerID = -1;                                  // To keep track of the host peer (first peer that connects to server)
};
