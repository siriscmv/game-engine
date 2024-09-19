#pragma once

#include <ZMQ/zmq.hpp>
#include <Entity.h>
#include <GameEngine.h>
#include <vector>
#include <map>

class Server {
public:
	Server(const std::vector<Entity*>& worldEntities, const std::vector<Entity*>& playerEntities);
	~Server();

	void initialize(int pubPort = 5555, int subPort = 5556, int reqPort = 5557);
	void run();

	GameEngine* getGameEngine() const;

private:
	std::vector<Entity*> _worldEntities;
	std::vector<Entity*> _playerEntities;                                // Initial spawn points of players
	std::vector<Entity*> _availablePlayerEntities;                       // Player entities that aren't assigned to a client yet
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
	std::string serializeEntity(const Entity& entity);
};