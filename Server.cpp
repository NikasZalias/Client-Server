#include "Server.h"
#include <iostream>
#include <algorithm>

using namespace std;

Server::Server(const int port) {
    if (!socket.create()) {
        throw SocketException("Failed to create a socket");
    } else if (!socket.bind(port)) {
        throw SocketException("Couldn't bind socket to port");
    } else if (!socket.listen()) {
        throw SocketException("Couldn't listen to socket");
    }
}

Server::~Server() {
	if (socket.isValid()) {
		cout << "Close server socket" << endl;
		socket.close();
	}
}

Server& Server::operator<<(const std::string &message) {
    socket << message;

    return *this;
}

Server& Server::operator>>(std::string &response) {
    socket >> response;

    return *this;
}

void Server::accept(Socket &socket) {
    if (!this->socket.accept(socket)) {
        throw SocketException("Server failed to accept connection on socket!");
    }
    connections.push_back(socket);
}

bool Server::removeConnection(Socket &socket) {
    vector<Socket>::iterator index = std::find(connections.begin(), connections.end(), socket);
    if (index != connections.end()) {
        connections.erase(index);
        return true;
    } else {
        return false;
    }
}

int Server::getConnectionCount() {
    return connections.size();
}
