#pragma once

#include <EventManager.h>

#include "Entity.h"
#include "Globals.h"
#include <vector>
#ifdef __APPLE__
#include <zmq.hpp>
#include <nlohmann/json.hpp>
#else
#include <ZMQ/zmq.hpp>
#include <JSON/json.hpp>
#endif

using json = nlohmann::json;

class Client {
public:
    Client();
    ~Client();

    void initialize(int pubPort = 5556, int entitySubPort = 5555, int reqPort = 5557, int hbPubPort = 5558, int subPort = 5559);
    bool handshakeWithServer();    
    void sendHeartbeatToServer();
    void sendInputToServer(const std::string& buttonPress);
    void receiveEntityUpdatesFromServer(EventManager *eventManager);
    void receiveMessagesFromServer();

    static Entity* deserializeEntity(const std::string& json);

    void setClientID(int id);
    int getClientID();
    int getEntityID();
    Position getViewOffset();
    void setGameState(GameState gameState);
    std::vector<Entity*>& getEntities();

    void setRefreshRate(RefreshRate rate = RefreshRate::SIXTY_FPS);
    RefreshRate getRefreshRate() const;
    int getRefreshRateMs() const;
    void setViewOffset(Position viewOffset);

private:
    zmq::context_t _context;
    zmq::socket_t _publisher;
    zmq::socket_t _heartbeatPublisher;
    zmq::socket_t _entitySubscriber;
    zmq::socket_t _subscriber;
    zmq::socket_t _requester;

    std::vector<Entity*> _entities;    

    int _clientID;
    int _entityID;
    Position _viewOffset;

    RefreshRate _refreshRate;
    int _refreshRateMs;

    GameState _gameState;
};
