#pragma once

#include "Entity.h"
#include <vector>
#include <map>
#ifdef __APPLE__
#include <zmq.hpp>
#else
#include <ZMQ/zmq.hpp>
#endif

class Peer {
public:
    Peer();
    ~Peer();

    void initialize(int reqPort = 5555, int serverPubPort = 5556);
    bool handshakeWithServer();
    void broadcastUpdates();
    void receiveUpdates();

    int getPeerId() const;
    void setPeerID(int id);
    std::vector<Entity*> getEntities() const;

    int _entityID;                                        //  Player entity ID assigned to this peer

private:
    zmq::context_t _context;
    zmq::socket_t _publisher;                             // PUB socket to broadcast entity updates to peers
    zmq::socket_t _subscriber;                            // SUB socket to receive entity updates from peers
    zmq::socket_t _requester;                             // REQ socket for the initial connection with the server
    int _peerID;
 
    std::vector<Entity*> _entities;
    std::map<int, int> _peerEntityMap; 
    std::vector<int> _knownPeers;                         // List of peer IDs that this peer is currently connected to

    std::vector<std::string> split(const std::string& str, char delim);
};
