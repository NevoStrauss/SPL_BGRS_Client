#include "../include/connectionHandler.h"
using boost::asio::ip::tcp;
using namespace std;
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;

//CTR
ConnectionHandler::ConnectionHandler(string host, short port): host_(host),
                                                                port_(port),
                                                                io_service_(),
                                                                socket_(io_service_),
                                                                promise(std::promise<bool>()){}
//DESTRUCTOR -> closing the socket
ConnectionHandler::~ConnectionHandler() {
    close();
}

/**
 * Creates connection with the Server with the endpoint(host, port)
 * @return true if the connection succeed, false otherwise.
 */
bool ConnectionHandler::connect() {
    std::cout << "Starting connect to "
              << host_ << ":" << port_ << std::endl;
    try {
        tcp::endpoint endpoint(boost::asio::ip::address::from_string(host_), port_); // the server endpoint
        boost::system::error_code error; // Initialize system error object
        socket_.connect(endpoint, error); // Connecting to Server
        if (error)
            throw boost::system::system_error(error);
    }
    catch (std::exception& e) {
        std::cerr << "Connection failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

/**
 * Reads the bytes from the socket Client's buffer,
 * that has been sent from the Server as a response to Clients message.
 * @param bytes char array to write into the data from the buffer.
 * @param bytesToRead how many bytes to read.
 * @return true if the reading succeed, false otherwise.
 */
bool ConnectionHandler::getBytes(char bytes[], unsigned int bytesToRead) {
    size_t tmp = 0;
    boost::system::error_code error;
    try {
        while (!error && bytesToRead > tmp ) {
            tmp += socket_.read_some(boost::asio::buffer(bytes+tmp, bytesToRead-tmp), error);
        }
        if(error)
            throw boost::system::system_error(error);
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

/**
 * Writes the encoded message to the buffer.
 * @param bytes: the encoded message converted to bytes array
 * @param bytesToWrite: how many bytes to write.
 * @return true if the writing succeed, false otherwise.
 */
bool ConnectionHandler::sendBytes(const char bytes[], int bytesToWrite) {
    int tmp = 0;
    boost::system::error_code error;
    try {
        while (!error && bytesToWrite > tmp ) {
            tmp += socket_.write_some(boost::asio::buffer(bytes + tmp, bytesToWrite - tmp), error);
        }
        if(error)
            throw boost::system::system_error(error);
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

/**
 * Method that called from the main thread, waiting for message from the Server.
 * @param line: the message has been received
 * @return try to decode the message and return true if succeeded, and false otherwise.
 */
bool ConnectionHandler::getLine(std::string& line) {
    return getFrameUTF8(line);
}

/**
 * Method called from the Executor thread after receiving a message to send from the Client.
 * @param line: the message to send.
 * @return try to encode the message and send it to the Server.
 *         return true if succeeded to send, and false otherwise.
 */
bool ConnectionHandler::sendLine(std::string& line) {
    return sendFrameUTF8(line, ' ');
}


/**
 * After receiving message from the Server, decoding the message.
 * @param frame the message to decode.
 * @return try to decode the message and return true if succeeded, and false otherwise.
 */
bool ConnectionHandler::getFrameUTF8(std::string& frame) {
    char ch[2]; // Initialize char array for storing the op_code
    try {
        short op_code;
        if(!getBytes(ch, 2)) // Decoding the op_code and write into {@code ch}
            return false;
        op_code = bytesToShort(ch);     // Converts the bytes to short
        if (op_code==13){               // Error message received
            return decodeError(frame);
        } else{                         // ACK message received
            return decodeAck(frame);
        }
    } catch (std::exception& e) {
        std::cerr << "recv failed2 (Error: " << e.what() << ')' << std::endl;
        return false;
    }
}

/**
 * Encodes and sends the Client's message.
 * @param frame: the message to encode and send.
 * @param delimiter: the separator char betwwen words.
 * @return true if succeeded to decode and send, false otherwise.
 */
bool ConnectionHandler::sendFrameUTF8(const std::string& frame, char delimiter) {
    vector<string> msgToEncode;
    string separator(1,delimiter); // Create new string with one delimiter.
    // boost::split method fill the {@code msgToEncode} by parsing the {@code frame}
    // and separate it to sub strings each time it encountered {@code separator}.
    boost::split(msgToEncode, frame, boost::is_any_of(separator));
    vector<char> msg = encodeMessage(msgToEncode); //Starts the encoding process
    if (msg.empty())
        return false;
    int size = msg.size();
    char message[size];  // Transfer the values in the vector<string> to char array.
    for (int i = 0; i < size; ++i) {
        message[i]= msg[i];
    }
    bool result=sendBytes(message,size); // Send the encoded message to the Server
    if(!result)
        return false;
    return true;
}

// Close down the connection properly.
void ConnectionHandler::close() {
    try{
        socket_.close();
    } catch (...) {
        std::cout << "closing failed: connection already closed" << std::endl;
    }
}

/**
 * Methods to convert bytes array to its short value.
 * @param bytesArr: the array to convert
 * @return the short value.
 * Thank you.
 */
short ConnectionHandler::bytesToShort(char *bytesArr) {
    short result = (short)((bytesArr[0] & 0xff) << 8);
    result += (short)(bytesArr[1] & 0xff);
    return result;
}

/**
 * Methods to convert short value to bytes array.
 * @param num: the short number to convert.
 * @param bytesArr the char array to fill.
 * Thank you.
 */
void ConnectionHandler::shortToBytes(short num, char *bytesArr) {
    bytesArr[0] = ((num >> 8) & 0xFF);
    bytesArr[1] = (num & 0xFF);
}

/**
 * Encodes messages of the commands: Admin/student registration, login.
 * @param message: the message to encode after detecting it's op_code.
 * @param OP_CODE: the current commands op_code.
 * @return the encoded message in vector<char>.
 */
vector<char> ConnectionHandler::createMsg2args(vector<string>& message, short OP_CODE){
    vector<char> output;            // Create the output vector.
    // Store the op_code in char array.
    char bytes[2];
    shortToBytes(OP_CODE,bytes);
    output.push_back(bytes[0]);
    output.push_back(bytes[1]);
    // Store the username received from the Client.
    string& username = message[1];
    for(unsigned int i=0;i<username.length();i++){
        output.push_back(username[i]);
    }
    output.push_back('\0');         // Insert '/0' delimiter.
    // Store the password received from the Client.
    string& password = message[2];
    for(unsigned int i=0; i<password.length();i++){
        output.push_back(password[i]);
    }
    output.push_back('\0');         // Insert '/0' delimiter.
    return output;
}

/**
 * Encodes messages of the commands: course registration/status, kdam check, is/un registration.
 * @param message: the message to encode after detecting it's op_code.
 * @param OP_CODE: the current commands op_code.
 * @return the encoded message in vector<char>.
 */
vector<char> ConnectionHandler::createMsg1arg(vector<string>& message, short OP_CODE){
    vector<char> output;            // Create the output vector.
    // Store the op_code in char array.
    char bytes[2];
    shortToBytes(OP_CODE,bytes);
    output.push_back(bytes[0]);
    output.push_back(bytes[1]);
    // Extract the short number of the course number.
    short courseNumber = boost::lexical_cast<short>(message[1]);
    shortToBytes(courseNumber,bytes);   // Convert the course number to bytes array and store it in {@code bytes}
    // Insert the encoded course number to the output vector.
    output.push_back(bytes[0]);
    output.push_back(bytes[1]);
    return output;
}

/**
 * Handles administrator registration Client request.
 * @param message: the message to encode with the username and password.
 * @return the encoded message in vector<char>.
 */
vector<char> ConnectionHandler::adminReg(vector<string>& message) {
    return createMsg2args(message,ADMINREG);
}

/**
 * Handles student registration Client request.
 * @param message: the message to encode with the username and password.
 * @return the encoded message in vector<char>.
 */
vector<char> ConnectionHandler::studentReg(vector<string>& message) {
    return createMsg2args(message,STUDENTREG);
}

/**
 * Handles login Client request.
 * @param message: the message to encode with the username and password.
 * @return the encoded message in vector<char>.
 */
vector<char> ConnectionHandler::login(vector<string>& message) {
    return createMsg2args(message,LOGIN);
}

/**
 * Handles logout Client request.
 * @param message: the message to encode with the logout op_code.
 * @return the encoded message in vector<char>.
 */
vector<char> ConnectionHandler::logout(vector<string>& message) {
    vector<char> output(0);         // Create the output vector.
    char bytes[2];
    shortToBytes(LOGOUT,bytes);     // Convert the logout op_code to bytes.
    // Insert the encoded op_code to the output vector.
    output.push_back(bytes[0]);
    output.push_back(bytes[1]);
    return output;
}

/**
 * Handles student course registration Client request.
 * @param message: the message to encode with the course number.
 * @return the encoded message in vector<char>.
 */
vector<char> ConnectionHandler::courseReg(vector<string>& message) {
    return createMsg1arg(message,COURSEREG);
}

/**
 * Handles student kdam course check Client request.
 * @param message: the message to encode with the course number to check.
 * @return the encoded message in vector<char>.
 */
vector<char> ConnectionHandler::kdamCheck(vector<string>& message) {
    return createMsg1arg(message,KDAMCHECK);
}

/**
 * Handles administrator course status check Client request.
 * @param message: the message to encode with the course number to check.
 * @return the encoded message in vector<char>.
 */
vector<char> ConnectionHandler::courseStat(vector<string>& message) {
    return createMsg1arg(message,COURSESTAT);
}

/**
 * Handles administrator student status check Client request.
 * @param message: the message to encode with the student name to check.
 * @return the encoded message in vector<char>.
 */
vector<char> ConnectionHandler::studentStat(vector<string>& message) {
    vector<char> output;            // Create the output vector.
    char bytes[2];
    // Store the converted op_code in the output vector.
    shortToBytes(STUDENTSTAT,bytes);
    output.push_back(bytes[0]);
    output.push_back(bytes[1]);
    string& username = message[1];   // Extract the username from the Client's message.
    // Transfer the username from the string into the output vector, and insert '\0' delimiter.
    for(unsigned int i=0;i<username.length();i++){
        output.push_back(username[i]);
    }
    output.push_back('\0');
    return output;
}

/**
 * Handles student course registration check Client request.
 * @param message: the message to encode.
 * @return the encoded message in vector<char>.
 */
vector<char> ConnectionHandler::isRegistered(vector<string>& message) {
    return createMsg1arg(message,ISREGIDTERED);
}

/**
 * Handles student course un-registration check Client request.
 * @param message: the message to encode.
 * @return the encoded message in vector<char>.
 */
vector<char> ConnectionHandler::unRegistered(vector<string>& message) {
    return createMsg1arg(message,UNREGISTER);
}

/**
 * Handles student courses status check Client request.
 * @param message: the message to encode.
 * @return the encoded message in vector<char>.
 */
vector<char> ConnectionHandler::myCourses(vector<string>& message) {
    vector<char> output(0);
    char output1[2];
    shortToBytes(MYCOURSES,output1);
    output.push_back(output1[0]);
    output.push_back(output1[1]);
    return output;
}

/**
 * A switch case method for handling each command by its op_code.
 * @param message: the message to encode.
 * @return the encoded message in vector<char>.
 */
vector<char> ConnectionHandler::whichOPCODE(vector<string>& message) {
    string& op_code = message[0];
    if (op_code == "ADMINREG")
        return adminReg(message);
    else if (op_code == "STUDENTREG")
        return studentReg(message);
    else if (op_code == "LOGIN")
        return login(message);
    else if (op_code == "LOGOUT")
        return logout(message);
    else if (op_code == "COURSEREG")
        return courseReg(message);
    else if (op_code == "KDAMCHECK")
        return kdamCheck(message);
    else if (op_code == "COURSESTAT")
        return courseStat(message);
    else if (op_code == "STUDENTSTAT")
        return studentStat(message);
    else if (op_code == "ISREGISTERED")
        return isRegistered(message);
    else if (op_code == "UNREGISTER")
        return unRegistered(message);
    else if (op_code == "MYCOURSES")
        return myCourses(message);
    else
        //return empty vector indicates that no matching op_code found.
        return vector<char>(0);
}

/**
 * Starts the encoding process of the message from the Client.
 * @param msgToEncode: the message to encode.
 * @return the encoded message in vector<char> or
 *         empty string if there is no matching op_code supported.
 */
vector<char> ConnectionHandler::encodeMessage (vector<string> msgToEncode){
    return whichOPCODE(msgToEncode);
}

/**
 * In case of Error occurs(sent back from the Server), decodes the message
 * for presenting the Client.
 * @param frame: an empty string to concat with the proper message for the Client.
 * @return true if the message received and decoded properly, false otherwise.
 */
bool ConnectionHandler::decodeError(std::string &frame) {
    frame.append("ERROR ");     // Insert the Error message.
    // Get the op_code from the message sent from the Server.
    char OP_CODE_BYTES[2];
    try{
        if (!getBytes(OP_CODE_BYTES, 2)){
            return false;
        }
        // Convert the op_code from bytes to its short value and add it to the frame.
        short OP_CODE = bytesToShort(OP_CODE_BYTES);
        frame.append(std::to_string(OP_CODE));
        return true;
    }catch (std::exception& e) {
        std::cerr << "recv failed2 (Error: " << e.what() << ')' << std::endl;
        return false;
    }
}

/**
 * In case of well submitted and answered request, decodes the message
 * for presenting the Client.
 * @param frame: an empty string to concat with the proper message for the Client.
 * @return true if the message received and decoded properly, false otherwise.
 */
bool ConnectionHandler::decodeAck(std::string &frame) {
    frame.append("ACK ");  // Insert the Acknowledge message.
    // Get the op_code from the message sent from the Server.
    char OP_CODE_BYTES[2];
    try{
        if (!getBytes(OP_CODE_BYTES, 2)){
            return false;
        }
        // Convert the op_code from bytes to its short value and add it to the frame.
        short OP_CODE = bytesToShort(OP_CODE_BYTES);
        frame.append(std::to_string(OP_CODE));
        // If the command demands more content to decode.
        if ((OP_CODE >= KDAMCHECK) & (OP_CODE != UNREGISTER)){
            frame.append("\n");
            return continueProcess(frame);
        }
        // Get the last byte sent from the Server.
        else{
            if (!getBytes(OP_CODE_BYTES, 1)){
                return false;
            }
        }
        return true;
    }catch (std::exception& e) {
        std::cerr << "recv failed2 (Error: " << e.what() << ')' << std::endl;
        return false;
    }
}

/**
 * In case of receiving ACK from the Server, and if the message is long
 * and demands more process to decode the bytes.
 * @param frame: the answer for the client during the process.
 * @return true if the message received and decoded properly, false otherwise.
 */
bool ConnectionHandler::continueProcess(std::string &frame) {
    char ch[1];         //Initializing char array for one byte.
    // Get the next byte from the message sent from the Server
    // and add it to the frame.
    do {
        if (!getBytes(ch, 1))
            return false;
        if (ch[0] != '\0')
            frame.append(1,ch[0]);
    }while (ch[0] != '\0');
    return true;
}

/**
 * @return the current promise object.
 */
promise<bool>& ConnectionHandler::getPromise() {return promise;}

/**
 * @param toSet: boll val to set the promise object for.
 */
void ConnectionHandler::setPromise(bool toSet) {promise.set_value(toSet);}

/**
 * Create new promise object and assigned it to the previous one.
 */
void ConnectionHandler::resetPromise() {promise = std::promise<bool>();}
