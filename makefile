CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS:=-lboost_system -lboost_thread -pthread

all: bin/BGRSClient
	g++ -o bin/BGRSClient bin/connectionHandler.o bin/BGRSclient.o $(LDFLAGS)

bin/BGRSClient: bin/connectionHandler.o bin/BGRSclient.o
@echo 'Building targer: Client'
@echo 'Invoking: C++ Linker'
	g++ -Wall -Weffc++ -o bin/BGRSClient

bin/connectionHandler.o: src/connectionHandler.cpp
	g++ $(CFLAGS) -o bin/connectionHandler.o src/connectionHandler.cpp

bin/BGRSclient.o: src/BGRSclient.cpp
	g++ $(CFLAGS) -o bin/BGRSclient.o src/BGRSclient.cpp

.PHONY: clean
clean:
	rm -f bin/*