#pragma once

#include <Entity.h>
#include <GameEngine.h>
#include <Globals.h>
#include <vector>
#include <map>
#ifdef __APPLE__
#include <zmq.hpp>
#include <nlohmann/json.hpp>
#else
#include <ZMQ/zmq.hpp>
#include <JSON/json.hpp>
#endif

using json = nlohmann::json;

class Server {
public:
	Server(const std::vector<Entity*>& worldEntities, const std::vector<Entity*>& spawnPoints);
	~Server();

	void initialize(int entityPubPort = 5555, int subPort = 5556, int reqPort = 5557, int hbSubPort = 5558, int pubPort = 5559);
	void run();

	GameEngine* getGameEngine() const;

	static std::string serializeEntity(const Entity& entity);
	void monitorHeartbeats();
	void handleClientDisconnect(int clientId);

	void setRefreshRate(RefreshRate rate = RefreshRate::SIXTY_FPS);
	RefreshRate getRefreshRate() const;
	int getRefreshRateMs() const;
	void setSimulationSpeed(double speed);
	void setHeartBeatTimeout(int milliseconds);
	

private:
	std::vector<Entity*> _worldEntities;
	std::vector<Entity*> _spawnPoints;                                   // Spawn points of players	
	std::vector<Entity*> _allEntities;                                   // All entities
	zmq::context_t _context;
	zmq::socket_t _entityPublisher;
	zmq::socket_t _publisher;
	zmq::socket_t _subscriber;
	zmq::socket_t _heartbeatSubscriber;
	zmq::socket_t _responder;	

	int _nextClientID;
	int _nextEntityID;                                                  // To track the proper ID when creating player entities
	std::map<int, Entity*> _clientMap;                                  // A map between client ID and assigned player entity

	GameEngine* _engine;

	void handleClientHandeshake();
	void listenToHeartbeatMessages();
	void listenToClientMessages();
	void processClientInput(int clientId, const std::string& buttonPress);
	void updateClientEntities();
	void broadcastDisconnect(int clientId);
	void broadcastNewConnection(Entity* entity);

	RefreshRate _refreshRate;
	int _refreshRateMs;

	// A map to track the last heartbeat time for each client
	std::unordered_map<int, std::chrono::time_point<std::chrono::steady_clock>> _lastHeartbeatMap; 
	std::chrono::milliseconds _heartbeatTimeout = std::chrono::milliseconds(1000);                  // 1 sec default timeout

};