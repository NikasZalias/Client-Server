OBJS = server_test.o client_test.o Socket.o Client.o Server.o
FILES = server_test.cpp client_test.cpp Socket.h Client.h Server.h

CC = g++
CFLAGS = -Wall -c -std=c++11
LFLAGS = -Wall -pthread

server: server_test.o Socket.o Server.o
	$(CC) $(LFLAGS) server_test.o Socket.o Server.o -o server

client: client_test.o Socket.o Client.o
	$(CC) $(LFLAGS) client_test.o Socket.o Client.o -o client

client_test.o: $(FILES)
	$(CC) $(CFLAGS) client_test.cpp

server_test.o: $(FILES)
	$(CC) $(CFLAGS) server_test.cpp

Client.o: Client.h Client.cpp
	$(CC) $(CFLAGS) Client.cpp

Server.o: Server.h Server.cpp
	$(CC) $(CFLAGS) Server.cpp

Socket.o: Socket.h Socket.cpp
	$(CC) $(CFLAGS) Socket.cpp

clean:
	rm *.o *~ server client
