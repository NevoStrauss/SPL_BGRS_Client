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
            std::string line(buf);
            cout <<line << endl;
            if (!connectionHandler.sendLine(line)) {
                std::cout << "Disconnected. Exiting...\n" << std::endl;
                break;
            }
            if (line == "LOGOUT"){
                if (connectionHandler.getPromise().get_future().get())
                    break;
                else
                    connectionHandler.resetPromise();
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

    Executor executor(connectionHandler);
    std::thread th1 (&Executor::run,&executor);
    while (true) {
        std::string answer;
        if (!connectionHandler.getLine(answer)) {
            std::cout << "Disconnected. Exiting...\n" << std::endl;
            break;
        }
        std::cout << answer << std::endl;
        if (answer == "ACK 4") {
            connectionHandler.setPromise(true);
            break;
        }
        if (answer == "ERROR 4")
            connectionHandler.setPromise(false);
    }
    th1.join();
    return 0;
}
