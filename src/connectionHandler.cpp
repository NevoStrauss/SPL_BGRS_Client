#include "../include/connectionHandler.h"
using boost::asio::ip::tcp;
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;


ConnectionHandler::ConnectionHandler(string host, short port): host_(host), port_(port), io_service_(), socket_(io_service_){}

ConnectionHandler::~ConnectionHandler() {
    close();
}

bool ConnectionHandler::connect() {
    std::cout << "Starting connect to "
              << host_ << ":" << port_ << std::endl;
    try {
        tcp::endpoint endpoint(boost::asio::ip::address::from_string(host_), port_); // the server endpoint
        boost::system::error_code error;
        socket_.connect(endpoint, error);
        if (error)
            throw boost::system::system_error(error);
    }
    catch (std::exception& e) {
        std::cerr << "Connection failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

bool ConnectionHandler::getBytes(char bytes[], unsigned int bytesToRead) {
    size_t tmp = 0;
    boost::system::error_code error;
    try {
        while (!error && bytesToRead > tmp ) {
            int i =0;
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

bool ConnectionHandler::getLine(std::string& line) {
    return getFrameUTF8(line, '\n');
}

bool ConnectionHandler::sendLine(std::string& line) {
    return sendFrameUTF8(line, ' ');
}

bool ConnectionHandler::getFrameUTF8(std::string& frame, char delimiter) {
    char ch[2];
    try {

        short op_code;
        if(!getBytes(ch, 2))
        {
            return false;
        }
        op_code = bytesToShort(ch);
        if (op_code==13){
            return decodeError(frame);
        } else{
            return decodeAck(frame);
        }
    } catch (std::exception& e) {
        std::cerr << "recv failed2 (Error: " << e.what() << ')' << std::endl;
        return false;
    }
}

bool ConnectionHandler::sendFrameUTF8(const std::string& frame, char delimiter) {
    vector<string> msgToEncode;
    string separator(1,delimiter);
    boost::split(msgToEncode, frame, boost::is_any_of(separator));
    vector<char> msg = encodeMessage(msgToEncode);
    int size = msg.size();
    char message[size];
    for (int i = 0; i < size; ++i) {
        message[i]= msg[i];
    }
    bool result=sendBytes(message,size);
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

short ConnectionHandler::bytesToShort(char *bytesArr) {
    short result = (short)((bytesArr[0] & 0xff) << 8);
    result += (short)(bytesArr[1] & 0xff);
    return result;
}

void ConnectionHandler::shortToBytes(short num, char *bytesArr) {
    bytesArr[0] = ((num >> 8) & 0xFF);
    bytesArr[1] = (num & 0xFF);
}

vector<char> ConnectionHandler::createMsg2args(vector<string>& message, short OP_CODE ){
    vector<char> output;
    char bytes[2];
    shortToBytes(OP_CODE,bytes);
    output.push_back(bytes[0]);
    output.push_back(bytes[1]);
    string& username = message[1];
    for(int i=0;i<username.length();i++){
        output.push_back(username[i]);
    }
    output.push_back('\0');
    string& password = message[2];
    for(int i=0; i<password.length();i++){
        output.push_back(password[i]);
    }
    output.push_back('\0');
    return output;
}

vector<char> ConnectionHandler::createMsg1arg(vector<string>& message, short OP_CODE){
    vector<char> output;
    char bytes[2];
    shortToBytes(OP_CODE,bytes);
    output.push_back(bytes[0]);
    output.push_back(bytes[1]);
    short courseNumber = boost::lexical_cast<short>(message[1]);
    shortToBytes(courseNumber,bytes);
    output.push_back(bytes[0]);
    output.push_back(bytes[1]);
    return output;
}

vector<char> ConnectionHandler::adminReg(vector<string>& message) {
    return createMsg2args(message,ADMINREG);
}

vector<char> ConnectionHandler::studentReg(vector<string>& message) {
    return createMsg2args(message,STUDENTREG);
}

vector<char> ConnectionHandler::login(vector<string>& message) {
    return createMsg2args(message,LOGIN);
}

vector<char> ConnectionHandler::logout(vector<string>& message) {
    vector<char> output(0);
    char bytes[2];
    shortToBytes(LOGOUT,bytes);
    output.push_back(bytes[0]);
    output.push_back(bytes[1]);
    return output;
}

vector<char> ConnectionHandler::courseReg(vector<string>& message) {
    return createMsg1arg(message,COURSEREG);
}

vector<char> ConnectionHandler::kdamCheck(vector<string>& message) {
    return createMsg1arg(message,KDAMCHECK);
}

vector<char> ConnectionHandler::courseStat(vector<string>& message) {
    return createMsg1arg(message,COURSESTAT);
}

vector<char> ConnectionHandler::studentStat(vector<string>& message) {
    vector<char> output;
    char bytes[2];
    shortToBytes(STUDENTSTAT,bytes);
    output.push_back(bytes[0]);
    output.push_back(bytes[1]);
    string& username = message[1];
    for(int i=0;i<username.length();i++){
        output.push_back(username[i]);
    }
    output.push_back('\0');
    return output;
}

vector<char> ConnectionHandler::isRegistered(vector<string>& message) {
    return createMsg1arg(message,ISREGIDTERED);
}

vector<char> ConnectionHandler::unRegistered(vector<string>& message) {
    return createMsg1arg(message,UNREGISTER);
}

vector<char> ConnectionHandler::myCourses(vector<string>& message) {
    vector<char> output(0);
    char output1[2];
    shortToBytes(MYCOURSES,output1);
    output.push_back(output1[0]);
    output.push_back(output1[1]);
    return output;
}

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
        throw "Unsupported operation";
}

vector<char> ConnectionHandler::encodeMessage (vector<string> msgToEncode){
    return whichOPCODE(msgToEncode);
}

bool ConnectionHandler::decodeError(std::string &frame) {
    frame.append("ERROR ");
    char OP_CODE_BYTES[2];
    try{
        if (!getBytes(OP_CODE_BYTES, 2)){
            return false;
        }
        short OP_CODE = bytesToShort(OP_CODE_BYTES);
        frame.append(std::to_string(OP_CODE));
        return true;
    }catch (std::exception& e) {
        std::cerr << "recv failed2 (Error: " << e.what() << ')' << std::endl;
        return false;
    }
}

bool ConnectionHandler::decodeAck(std::string &frame) {
    frame.append("ACK ");
    char OP_CODE_BYTES[2];
    try{
        if (!getBytes(OP_CODE_BYTES, 2)){
            return false;
        }
        short OP_CODE = bytesToShort(OP_CODE_BYTES);
        frame.append(std::to_string(OP_CODE));
        if (OP_CODE >= KDAMCHECK & OP_CODE != UNREGISTER){
            frame.append(" ");
            return continueProcess(frame);
        }
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

bool ConnectionHandler::continueProcess(std::string &frame) {
    char ch[1];
    do {
        if (!getBytes(ch, 1))
            return false;
        if (ch[0] != '\0')
            frame.append(1,ch[0]);
    }while (ch[0] != '\0');
    return true;
}
