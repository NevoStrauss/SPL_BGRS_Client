#include <stdlib.h>
#include "../include/connectionHandler.h"
using namespace std;
/**
* This code assumes that the server replies the exact text the client sent it (as opposed to the practical session example)
*/

class Sender{
private:
    ConnectionHandler& connectionHandler;
public:
    Sender (ConnectionHandler& _connectionHandler) : connectionHandler(_connectionHandler){}

    void run(){
        while(true) {
            const short bufsize = 1024;
            char buf[bufsize];
            std::cin.getline(buf, bufsize);
            std::string line(buf);
            cout << "Client> " << line << endl;
            if (!connectionHandler.sendLine(line)) {
                std::cout << "Disconnected. Exiting...\n" << std::endl;
                break;
            }
            cout << "Succeeded sending message" << endl;
        }
    }
};

class Reciever{
private:
    ConnectionHandler& connectionHandler;
    std::thread& sender;
public:
    Reciever(ConnectionHandler& _connectionHandler, std::thread& _sender):connectionHandler(_connectionHandler),
                                                                                            sender(_sender) {}

    void run(){
        while(true) {
            std::string answer;
            if (!connectionHandler.getLine(answer)) {
                std::cout << "Disconnected. Exiting...\n" << std::endl;
            }
            std::cout << "Client< " << answer << std::endl;
            if (answer == "ACK 4") {
                std::cout << "Exiting...\n" << std::endl;
                sender.detach();
                break;
            }
        }
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
    Sender sender(connectionHandler);

    std::thread th1 (&Sender::run,&sender);
    Reciever reciever(connectionHandler,th1);
    std::thread th2 (&Reciever::run,&reciever);
    th1.join();
    if (!th2.joinable())
        th2.detach();
    return 0;
}
