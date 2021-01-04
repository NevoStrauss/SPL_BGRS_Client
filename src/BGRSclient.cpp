#include <stdlib.h>
#include "../include/connectionHandler.h"
using namespace std;

class Executor{
private:
    ConnectionHandler& connectionHandler;
public:
    Executor (ConnectionHandler& _connectionHandler) : connectionHandler(_connectionHandler){}
    void run(){
        while(true) {
            const short bufsize = 1024;
            char buf[bufsize];
            std::cin.getline(buf, bufsize);
            cout << "2" << endl;
            std::string line(buf);
            cout << "3" << endl;
            cout << "Client#1> " << line << endl;
            cout << "4" << endl;
            if (!connectionHandler.sendLine(line)) {
                std::cout << "Disconnected. Exiting...\n" << std::endl;
                break;
            }
            if (line == "LOGOUT") {
                cout << "Logging out..." << endl;
                break;
            }
        }
        cout<<"finished"<<endl;
    }
};

int main (int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " host port" << std::endl << std::endl;
        return -1;
    }
    std::string host = argv[1];
    short port = atoi(argv[2]);

    ConnectionHandler connectionHandler(host, port);
    if (!connectionHandler.connect()) {
        std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
        return 1;
    }
    std::cerr << "Connected to  " << host << ":" << port << std::endl;
    Executor executor(connectionHandler);
    std::thread th1 (&Executor::run,&executor);
    try {
        while (true) {
            std::string answer;
            if (!connectionHandler.getLine(answer)) {
                std::cout << "Disconnected. Exiting...\n" << std::endl;
                break;
            }
            std::cout << "Client#1< " << answer << std::endl;
            if (answer == "ACK 4") {
                std::cout << "Exiting...\n" << std::endl;
            }
        }
        cout << "joining" << endl;
        th1.join();
        cout << "rerutn0" << endl;
        return 0;
    } catch (exception& e) {
        cout<<"failed in something: " <<e.what()<<endl;
        return 0;
    }
}
