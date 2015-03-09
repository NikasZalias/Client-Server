#ifndef SOCKET_H
#define SOCKET_H

#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>

const int MAX_HOSTNAME_LENGTH = 16;
const int MAX_CONNECTIONS = 2000;
const int MAX_RECEIVABLE_LENGTH = 1024;

struct SocketException : public std::exception {
    std::string msg;

    SocketException(const std::string &message) : msg(message) {}
};

inline std::ostream& operator<<(std::ostream& o, const SocketException &e) {
    return o << e.msg;
}

class Socket {
private:
    int socketFD = -1; // Socket File Descriptor (as returned by socket())
    sockaddr_in socketAddress; //The sockaddr_in structure is used to store addresses for the Internet address family.

public:
    Socket();
    virtual ~Socket();

public:
    // Initialization
    bool create();
    void close();
    bool bind(const int port);
    bool listen() const;
    bool accept(Socket &newSocket) const;
    bool connect(const std::string &host, const int port);

    // Transmission
    bool send(const std::string &message) const;
    int receive(std::string &target);

    bool validateHost(const std::string &host) const;
    bool validatePort(const int port) const;
    bool isValid() const { return socketFD != -1; }

    Socket& operator<<(const std::string &message);
    Socket& operator>>(std::string &response);
    Socket& operator=(const Socket &otherSocket);
    bool operator==(const Socket &otherSocket);
protected:
private:
};

#endif // SOCKET_H
