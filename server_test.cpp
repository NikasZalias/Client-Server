#include <iostream>
#include "Server.h"
#include "Socket.h"
#include <pthread.h>
#include <thread>
#include <chrono>
#include <cstdlib>

using namespace std;

// Server creates a listening socket
Server *server;

void* echo(void *ptr) {
    cout << "start listening to client " << endl;
    // Copy the socket and free the space used by previous pointer
    Socket socket = *((Socket*) ptr);
    delete (Socket*) ptr;

    // Loop reading from client until an exception is thrown
    try {
        while(true) {
            string message, tmpMessage;
            int status = socket.receive(message);
            // Concat responses if the packet received has max length
            while (status == MAX_RECEIVABLE_LENGTH) {
                status = socket.receive(tmpMessage);
                message += tmpMessage;
            }
            // Check if peer shutdown message was received, otherwise echo the message back
            if (status == 0) {
                break;
            }

            // Echo received message
            socket.send("Echo: " + message);
        }
    } catch (SocketException &e) {
        cout << "Caught while listening to client: " << e << endl;
    }

    // Close the socket and remove from server connections array
    socket.close();
    if (!server->removeConnection(socket)) {
        cout << "Socket not found in array" << endl;
    }

    // Close the socket and thread
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
			cout << "Error wrong  arg count" << endl;
			return 1;
	}
	server = new Server(atoi(argv[1]));
	
    // Listen for client connections until an exception is thrown
    try {
        while(true) {
            //if (server->getConnectionCount() >= MAX_CONNECTIONS) {
                //cout << "All connection slots are taken, will check again in 5 seconds." << endl;
                //// Max client count reached, try again in 5 seconds
                //std::this_thread::sleep_for (std::chrono::seconds(5));
                //continue;
            //} else {
                //cout << "Found a free connection slot. Listening for client connections." << endl;
            //}

            // Connection slot is available, listen
            Socket *socket = new Socket();
            server->accept(*socket); // blocking call

            // Spawn a new thread to listen to the newly accepted socket
            pthread_t thread;
            if (pthread_create(&thread, NULL, echo, socket)) {
                throw SocketException("Unable to create thread!");
            }
        }
    } catch (SocketException &e) {
        cout << "Caught while listening for client connections: " << e << endl;
    }


    return 0;
}
