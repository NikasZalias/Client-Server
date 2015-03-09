#include <iostream>
#include "Client.h"
#include <thread>
#include <chrono>
#include <cstdlib>

using namespace std;

int main(int argc, char *argv[]) {
    try {
		if (argc != 3) {
			cout << "Error wrong arg count. Please enter the ./client HOST PORT." << endl;
			return 0;
		}
        Client client(argv[1], atoi(argv[2]));

        string input;
        // Read line from user
        getline(cin, input);
        while (!input.empty()) {
            // Send line via socket
            client << input;

            // Receive response
            string response, tmpResponse;
            int status = client.socket.receive(response);
            // Concat responses if the packet received has max length
            while (status == MAX_RECEIVABLE_LENGTH) {
                status = client.socket.receive(tmpResponse);
                response += tmpResponse;
            }

            // Output response
            cout << response << endl;

            // Read another line from user
            getline(cin, input);
        }
    } catch (SocketException &e) {
        cout << "Caught: " << e << endl;
    }

    return 0;
}
