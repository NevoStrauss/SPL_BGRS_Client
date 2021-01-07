#ifndef CONNECTION_HANDLER__
#define CONNECTION_HANDLER__

#include <string>
#include <iostream>
#include <vector>
#include <boost/asio.hpp>
using namespace std;
using boost::asio::ip::tcp;

/**
 * The connection handler object is responsible of the connection between the Client's and Server's socket.
 * It has the below members:
 * @host_: For the localhost address.
 * @port_: For the port to connect with.
 * @io_service: Provides core I/O functionality for the socket.
 * @socket: Basic stream socket object
 * @promise: Provides a facility to store a value(bool) that is later
 *           acquired asynchronously via its future object.
 *           it responsible of synchronizing the termination of the Executor thread
 *           and the main thread.
 */

class ConnectionHandler {
private:
    const std::string host_;
    const short port_;
    boost::asio::io_service io_service_;
    tcp::socket socket_;
    std::promise<bool> promise;

    /**
     * Public fields and methods:
     */
public:
    //CTR
    ConnectionHandler(std::string host, short port);

    //COPY CTR
    ConnectionHandler(const ConnectionHandler& connectionHandler);

    //DESTRUCTOR
    virtual ~ConnectionHandler();

    // Connect to the remote machine
    bool connect();

    // Read a fixed number of bytes from the server - blocking.
    // Returns false in case the connection is closed before bytesToRead bytes can be read.
    bool getBytes(char bytes[], unsigned int bytesToRead);

    // Send a fixed number of bytes from the client - blocking.
    // Returns false in case the connection is closed before all the data is sent.
    bool sendBytes(const char bytes[], int bytesToWrite);

    // Read an ascii line from the server
    // Returns false in case connection closed before a newline can be read.
    bool getLine(std::string& line);

    // Send an ascii line from the server
    // Returns false in case connection closed before all the data is sent.
    bool sendLine(std::string& line);

    // Get Ascii data from the server until the delimiter character
    // Returns false in case connection closed before null can be read.
    bool getFrameUTF8(std::string& frame);

    // Send a message to the remote host.
    // Returns false in case connection is closed before all the data is sent.
    bool sendFrameUTF8(const std::string& frame, char delimiter);

    // Close down the connection properly.
    void close();

    // Gets the current promise object
    std::promise<bool>& getPromise();

    // Sets the current state of the promise to {@code toSet}
    void setPromise(bool toSet);

    // Creates a new promise object (by overriding the previous)
    void resetPromise();

    /**
     * Private fields and methods:
     */
private:
    // A commands number serialization for common language.
    short ADMINREG = 1;
    short STUDENTREG = 2;
    short LOGIN = 3;
    short LOGOUT = 4;
    short COURSEREG = 5;
    short KDAMCHECK = 6;
    short COURSESTAT = 7;
    short STUDENTSTAT = 8;
    short ISREGIDTERED = 9;
    short UNREGISTER = 10;
    short MYCOURSES = 11;
    short ACK = 12;
    short ERROR = 13;

    // Converts bytes array to short value
    static short bytesToShort(char* bytesArr);

    // Converts short number to bytes array
    static void shortToBytes(short num, char* bytesArr);

    // Creates the proper message for administrator registration request
    vector<char> adminReg(vector<string>& message);

    // Creates the proper message for student registration request
    vector<char> studentReg(vector<string>& message);

    // Creates the proper message for login request
    vector<char> login(vector<string>& message);

    // Creates the proper message for logout request
    vector<char> logout(vector<string>& message);

    // Creates the proper message for students course registration request
    vector<char> courseReg(vector<string>& message);

    // Creates the proper message for students kdam courses check request
    vector<char> kdamCheck(vector<string>& message);

    // Creates the proper message for administrator course status request
    vector<char> courseStat(vector<string>& message);

    // Creates the proper message for administrator student status request
    vector<char> studentStat(vector<string>& message);

    // Creates the proper message for student registration for course check request
    vector<char> isRegistered(vector<string>& message);

    // Creates the proper message for student unregister for course request
    vector<char> unRegistered(vector<string>& message);

    // Creates the proper message for student courses status request
    vector<char> myCourses(vector<string>& message);

    // Recognize the OP_CODE and generates the proper method to handle
    vector<char> whichOPCODE(vector<string>& message);

    // Final creation of the message for two arguments commands like:
    // Admin/student registration, login..
    vector<char> createMsg2args(vector<string>& message, short OP_CODE);

    // Final creation of the message for one argument commands like:
    // course registration/status, kdam check, is/un registration..
    vector<char> createMsg1arg(vector<string>& message, short OP_CODE);

    // Starts the encoding process.
    vector<char> encodeMessage(vector<string> msgToEncode);

    // In case of Error occurs(sent back from the Server), decodes the message
    // for presenting the Client.
    bool decodeError(std::string& frame);

    // In case of well submitted and answered request, decodes the message
    // for presenting the Client.
    bool decodeAck(std::string& frame);

    // In case of receiving ACK from the Server, and if the message is long
    // and demands more process to decode the bytes.
    bool continueProcess(std::string& frame);

}; //class ConnectionHandler

#endif