#include <stdlib.h>
#include "../include/connectionHandler.h"
using namespace std;

/**
 * Runnable object executed by a separated thread.
 * Responsible of receiving the input message from the Client, decode and send it to the Server.
 * It has reference to the connection handler for using its encoding methods and IO service.
 */
class Executor{
private:
    ConnectionHandler& connectionHandler;
public:
    Executor (ConnectionHandler& _connectionHandler) : connectionHandler(_connectionHandler){}
    /**
     * Method to receive the input message from the Client and send it to the Server.
     */
    void run(){
        while(true) {
            // Initializing char array in size 1kb.
            const short bufsize = 1024;
            char buf[bufsize];
            // Receive input message from Client
            std::cin.getline(buf, bufsize);
            std::string line(buf);
            // Try to send the message(after encoding) to the Server.
            // return true if the action succeed, or false otherwise.
            if (!connectionHandler.sendLine(line)) {
                std::cout << "Disconnected. Exiting...\n" << std::endl;
                break;
            }
            // If Client sent LOGOUT, it gets in the promise blocking method (get_future().get())blocking,
            // until the main thread set a new value to the promise object.
            // if the action (LOGOUT) succeed, the value will be true and the thread finished.
            // if the action didn't succeed, the value will be false and it reset the promise
            //to a new promise, and continue its loop.
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
    // Check input arguments correctness.
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " host port" << std::endl << std::endl;
        return -1;
    }
    // Extract host address nd port from input arguments.
    std::string host = argv[1];
    short port = atoi(argv[2]);

    // Create connection handler for the Client.
    ConnectionHandler connectionHandler(host, port);
    // Try to connect the connection handler with the Server.
    if (!connectionHandler.connect()) {
        std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
        return 1;
    }
    std::cerr << "Connected to  " << host << ":" << port << std::endl;

    // Create the Runnable Executor object to run by the second thread,
    // and give it a reference to the connection handler of the Client.
    Executor executor(connectionHandler);
    // Execute the "Executor" thread (will activate the run method).
    std::thread th1 (&Executor::run,&executor);

    // main thread loop:
    while (true) {
        std::string answer;
        // Try to read messages from the Server.
        // return false if Exception has been thrown.
        if (!connectionHandler.getLine(answer)) {
            std::cout << "Disconnected. Exiting...\n" << std::endl;
            break;
        }
        // Display to the Client  the Message sent back from the Server.
        std::cout << answer << std::endl;
        // Client sent LOGOUT and action succeed.
        if (answer == "ACK 4") {
            // Set the promise object future value to true,
            // to resque the Executor thread from its blocking method,
            // and inform that it needs to terminate.
            // and break the loop.
            connectionHandler.setPromise(true);
            break;
        }
        // Client sent LOGOUT and action did not succeed.
        if (answer == "ERROR 4")
            // Set the promise object future value to false,
            // to resque the Executor thread from its blocking method,
            // and inform the that it needs to continue.
            // and continue the loop.
            connectionHandler.setPromise(false);
    }
    // Gets here only if LOGOUT action succeed.
    th1.join();
    return 0;
}
