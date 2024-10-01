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

    bool isHost() const;
    int getPlayerEntityID() const;
    std::vector<Entity*> getWorldEntities() const;
    std::vector<Entity*> getPlayerEntities() const;
    std::vector<Entity*> getEntitiesToProcess() const;

    void setRefreshRate(RefreshRate rate = RefreshRate::SIXTY_FPS);
    RefreshRate getRefreshRate() const;
    int getRefreshRateMs() const;

private:
    zmq::context_t _context;
    zmq::socket_t _publisher;                             // PUB socket to broadcast entity updates to peers
    zmq::socket_t _subscriber;                            // SUB socket to receive entity updates from peers
    zmq::socket_t _requester;                             // REQ socket for the initial connection with the server
    int _peerID;
    int _entityID;                                        //  Player entity ID assigned to this peer
    bool _isHost = false;
 
    std::vector<Entity*> _worldEntities;
    std::vector<Entity*> _playerEntities;
    std::vector<Entity*> _entities;
    std::map<int, int> _peerEntityMap;                    // Map between known peers and their player entities
    std::vector<int> _knownPeers;                         // List of peer IDs that this peer is currently connected to

    std::vector<std::string> split(const std::string& str, char delim);

    RefreshRate _refreshRate;
    int _refreshRateMs;
};
