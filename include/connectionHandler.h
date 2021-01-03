#ifndef CONNECTION_HANDLER__
#define CONNECTION_HANDLER__

#include <string>
#include <iostream>
#include <vector>
#include <boost/asio.hpp>
using namespace std;

using boost::asio::ip::tcp;

class ConnectionHandler {
private:
    const std::string host_;
    const short port_;
    boost::asio::io_service io_service_;   // Provides core I/O functionality
    tcp::socket socket_;

public:
    ConnectionHandler(std::string host, short port);
    ConnectionHandler(const ConnectionHandler& connectionHandler);
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
    bool getFrameUTF8(std::string& frame, char delimiter);

    // Send a message to the remote host.
    // Returns false in case connection is closed before all the data is sent.
    bool sendFrameUTF8(const std::string& frame, char delimiter);

    // Close down the connection properly.
    void close();

private:
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

    static short bytesToShort(char* bytesArr);
    static void shortToBytes(short num, char* bytesArr);
    vector<char> adminReg(vector<string>& message);
    vector<char> studentReg(vector<string>& message);
    vector<char> login(vector<string>& message);
    vector<char> logout(vector<string>& message);
    vector<char> courseReg(vector<string>& message);
    vector<char> kdamCheck(vector<string>& message);
    vector<char> courseStat(vector<string>& message);
    vector<char> studentStat(vector<string>& message);
    vector<char> isRegistered(vector<string>& message);
    vector<char> unRegistered(vector<string>& message);
    vector<char> myCourses(vector<string>& message);
    vector<char> whichOPCODE(vector<string>& message);
    vector<char> createMsg2args(vector<string>& message, short OP_CODE);
    vector<char> createMsg1arg(vector<string>& message, short OP_CODE);
    vector<char> encodeMessage  (vector<string> msgToEncode);
    bool decodeError(std::string& frame);
    bool decodeAck(std::string& frame);
    bool continueProcess(std::string& frame);

}; //class ConnectionHandler

#endif