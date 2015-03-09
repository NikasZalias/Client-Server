#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include "Socket.h"

class Client {
public:
    Socket socket;

public:
    Client(std::string ip, const int port);
    virtual ~Client();

public:
    Client& operator<<(const std::string &message);
    Client& operator>>(std::string &response);

protected:
private:
};

#endif // CLIENT_H
