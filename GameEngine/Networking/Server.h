#pragma once

#include <Entity.h>
#include <GameEngine.h>
#include <Globals.h>
#include <vector>
#include <map>
#include <JSON/json.hpp>
using json = nlohmann::json;
#ifdef __APPLE__
#include <zmq.hpp>
#else
#include <ZMQ/zmq.hpp>
#endif

class Server {
public:
	Server(const std::vector<Entity*>& worldEntities, const std::vector<Entity*>& spawnPoints);
	~Server();

	void initialize(int pubPort = 5555, int subPort = 5556, int reqPort = 5557);
	void run();

	GameEngine* getGameEngine() const;

	static std::string serializeEntity(const Entity& entity);

	void setRefreshRate(RefreshRate rate = RefreshRate::SIXTY_FPS);
	RefreshRate getRefreshRate() const;
	int getRefreshRateMs() const;
	void setSimulationSpeed(double speed);

private:
	std::vector<Entity*> _worldEntities;
	std::vector<Entity*> _spawnPoints;                                   // Spawn points of players	
	std::vector<Entity*> _allEntities;                                   // All entities
	zmq::context_t _context;
	zmq::socket_t _publisher;
	zmq::socket_t _subscriber;
	zmq::socket_t _responder;	

	int _nextClientID;
	std::map<int, Entity*> _clientMap;                                  // A map between client ID and assigned player entity

	GameEngine* _engine;

	void handleClientHandeshake();		
	void listenToClientMessages();
	void processClientInput(int clientId, const std::string& buttonPress);
	void updateClientEntities();

	RefreshRate _refreshRate;
	int _refreshRateMs;
};