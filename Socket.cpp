#include "Socket.h"

#include <string>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <stdexcept>
#include <netinet/tcp.h>

using namespace std;

Socket::Socket() {

}

Socket::~Socket() {

}

void Socket::close() {
    if (isValid()) {
        if (::close(socketFD) == -1) {
            cout << "Failed to close socket: " << strerror(errno) << endl;
        }
    }
}

bool Socket::validatePort(const int port) const {
    if (port < 1 || port > 65535) {
        return false;
    } else {
        return true;
    }
}

bool Socket::validateHost(const string &host) const {
    if (host.empty()) {
        return false;
    } else {
        return true;
    }
}

// Create a socket
bool Socket::create() {
    socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (!isValid()) return false;

    // To manipulate options at the sockets API level, level is specified as SOL_SOCKET
    // Options:
    // SO_REUSEADDR - allows multiple sockets to bind to the same source (e.g. 0.0.0.0 and 192.168.0.1)
        // If another socket is already in TIME_WAIT state binding to the same Addr:Port is possible
            // TIME_WAIT - lingering - sending residual data that wasn't when the socket was open
    // SO_REUSEPORT - Enables binding to the same source and port combination,
        // but requires both sockets to have this option prior to binding them
        // It was added later than SO_REUSEADDR so it's not used that often

    int optval = 1;
    if (setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval) == 0) { //The setsockopt function sets a socket option.
        return true;
    } else {
        cout << "::setsockopt in Socket::create error: " << strerror(errno) << endl;
        return false;
    }
}

// The destination address and port are set with the connect() function.
bool Socket::connect(const string &host, const int port) {
    if (!isValid()) return false;

    if (!validateHost(host)) {
        throw invalid_argument("Must specify a valid IP address!");
    } else if (!validatePort(port)) {
        throw invalid_argument("Wrong port specified: " + to_string(port) +
                               ". Must be in range [1..65535].");
    }

    socketAddress.sin_family = AF_INET;
    socketAddress.sin_port = htons(port); //htons -convert values between host and network byte order

    // Convert IPv4/IPv6 addresses from text to binary form
    inet_pton(AF_INET, host.c_str(), &socketAddress.sin_addr);

    if (errno == EAFNOSUPPORT) return false;

    if (::connect(socketFD, (sockaddr*) &socketAddress, sizeof socketAddress) == 0) {
        return true;
    } else {
        cout << "::connect error: " << strerror(errno) << endl;
        return false;
    }
}

// The source address and port are set with the bind() function.
bool Socket::bind(const int port) {
    if (!isValid()) return false;

    // AF_INET      - Address Family Internet (family for IPv4)
    // INADDR_ANY   - Any ip address
    // htons        - Converts unsigned short integer from host byte order to network byte order
        // (host byte order is LSB first, while network is MSB first)
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_addr.s_addr = INADDR_ANY;
    socketAddress.sin_port = htons(port);

    if (::bind(socketFD, (sockaddr*) &socketAddress, sizeof socketAddress) == 0) {
        return true;
    } else {
        cout << "::bind error: " << strerror(errno) << endl;
        return true;
    }

    // 0                            - any port
    // 0.0.0.0 (IPv4) OR :: (IPv6)  - any source address

    // Socket can be bound to any address which means all source IP addresses of all local interfaces.
    // If that socket is connected later on, the system has to choose a specific source IP address,
    // since a socket cannot be connected and at the same time be bound to any local IP address.

    // By default, no two sockets can be bound to the same combination of source address and source port.
    // A:P, B:Q where A,B = hosts, P,Q = ports. Bind is successful if (P != Q || A != B).
}

int Socket::receive(string &target) {
    target.resize(MAX_RECEIVABLE_LENGTH+1);
    int status = ::recv(socketFD, &target[0], MAX_RECEIVABLE_LENGTH, 0);
    if (status < 0) {
        cout << "::recv status " << status << ". Error: " << strerror(errno) << endl;
    }
    return status;
}

bool Socket::send(const string &msg) const {
    if (::send(socketFD, msg.c_str(), msg.size(), MSG_NOSIGNAL) == -1) {
        cout << "::send error: " << strerror(errno) << endl;
        return false;
    } else {
        return true;
    }
}

bool Socket::accept(Socket &newSocket) const {
    int addrLen = sizeof socketAddress;
    // Extracts the first connection request on the queue of pending connections for the
    // listening socket, socketFD, creates a new connected socket, and returns
    // a new file descriptor referring to that socket
    newSocket.socketFD = ::accept(socketFD, (sockaddr*) &socketAddress, (socklen_t*) &addrLen);

    if (newSocket.socketFD <= 0) {
        return false;
    } else {
        return true;
    }
}

bool Socket::listen() const {
    if (!isValid()) return false;

    if (::listen(socketFD, MAX_CONNECTIONS) == 0) {
        return true;
    } else {
        cout << "::listen error: " << strerror(errno) << endl;
        return false;
    }
}

Socket& Socket::operator<<(const std::string &message) {
    if (!send(message)) {
        throw SocketException("Failed to send message: " + message);
    }

    return *this;
}

Socket& Socket::operator>>(std::string &response) {
    if (receive(response) < 0) {
        throw SocketException("Failed to retrieve the response!");
    }

    return *this;
}

bool Socket::operator==(const Socket &otherSocket) {
    if (socketFD == otherSocket.socketFD) {
        return true;
    } else {
        return false;
    }
}

Socket& Socket::operator=(const Socket &otherSocket) {
    if (this != &otherSocket) {
        socketFD = otherSocket.socketFD;
        socketAddress = otherSocket.socketAddress;
    }

    return *this;
}
