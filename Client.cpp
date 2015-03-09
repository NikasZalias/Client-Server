#include "Client.h"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace std;

Client::Client(string ip, const int port) {
    if (!socket.create()) {
        throw SocketException("Failed to create a socket");
    } else if (!socket.connect(ip, port)) {
        throw SocketException("Client couldn't connect to " + ip + ":" + to_string(port));
    }
}

Client::~Client() {
    cout << "Close client socket" << endl;
    socket.close();
}

Client& Client::operator<<(const std::string &message) {
    socket << message;

    return *this;
}

Client& Client::operator>>(std::string &response) {
    socket >> response;

    return *this;
}
