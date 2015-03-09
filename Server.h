#ifndef SERVER_H
#define SERVER_H

#include "Socket.h"
#include <vector>

class Server {
private:
    Socket socket;
    std::vector<Socket> connections; //for multiple connections

public:
    Server(const int port);
	Server() {}
    virtual ~Server();

public:
    Server& operator<<(const std::string &message);
    Server& operator>>(std::string &response);
    int getConnectionCount();
    bool removeConnection(Socket &socket);
    void accept(Socket &socket);
protected:
private:
};

#endif // SERVER_H
