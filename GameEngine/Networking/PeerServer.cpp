#include "PeerServer.h"

#include <Client.h>
#include <iostream>
#include <Server.h>
#include <string>
#ifdef __APPLE__
#include <zmq.hpp>
#else
#include <ZMQ/zmq.hpp>
#endif

PeerServer::PeerServer() {
	_context = zmq::context_t(1);
	_publisher = zmq::socket_t(_context, zmq::socket_type::pub);
	_responder = zmq::socket_t(_context, zmq::socket_type::rep);
}

constexpr int START_ID = 10000;
int currentId = START_ID;

PeerServer::~PeerServer() {
	_publisher.close();
}

void PeerServer::initialize() {
	_responder.bind("tcp://*:" + std::to_string(currentId));
	currentId += 1;
	_publisher.connect("tcp://localhost:" + std::to_string(currentId));
}


void PeerServer::run() {
	zmq::message_t request;

	if (_responder.recv(request, zmq::recv_flags::dontwait)) {
		std::string clientRequest(static_cast<char*>(request.data()), request.size());
		std::ostringstream message;

		if (clientRequest == "CONNECT") {
			currentId += 1;
			const int peerId = currentId;

			message << peerId << "|";

			for (int i = peerId-1; i >= START_ID; i--) {
				message << i << "|";
			}

			std::ostringstream broadcastStream;
			broadcastStream << "new_peer|" << peerId;
			const std::string broadcastString = broadcastStream.str();
			zmq::message_t broadcastMessage(broadcastString.size());
			memcpy(broadcastMessage.data(), broadcastString.c_str(), broadcastString.size());
			_publisher.send(broadcastMessage, zmq::send_flags::none);

		} else if (clientRequest == "ENTITIES") {
			for (auto _entity: _entities) {
				message << Server::serializeEntity(*_entity) << "|";
			}
		} else {
			throw std::runtime_error("Invalid request");
		}

		const std::string response = message.str();
		zmq::message_t reply(response.size());
		memcpy(reply.data(), response.c_str(), response.size());
		_responder.send(reply, zmq::send_flags::none);
	}
}


std::vector<Entity*> PeerServer::getEntities() const { return _entities; }