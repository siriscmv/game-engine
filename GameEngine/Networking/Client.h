#pragma once

#include "Entity.h"
#include "Globals.h"
#include <vector>
#include <JSON/json.hpp>
using json = nlohmann::json;
#ifdef __APPLE__
#include <zmq.hpp>
#else
#include <ZMQ/zmq.hpp>
#endif

class Client {
public:
    Client();
    ~Client();

    void initialize(int pubPort = 5556, int subPort = 5555, int reqPort = 5557);
    bool handshakeWithServer();
    void sendInputToServer(const std::string& buttonPress);
    void receiveUpdatesFromServer();

    static Entity* deserializeEntity(const std::string& json);

    void setClientID(int id);
    int getClientID();
    int getEntityID();
    Position getViewOffset();
    void setGameState(GameState gameState);
    std::vector<Entity*> getEntities() const;

    void setRefreshRate(RefreshRate rate = RefreshRate::SIXTY_FPS);
    RefreshRate getRefreshRate() const;
    int getRefreshRateMs() const;
    void setViewOffset(Position viewOffset);

private:
    zmq::context_t _context;
    zmq::socket_t _publisher;
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
