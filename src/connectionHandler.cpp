#include "../include/connectionHandler.h"
using boost::asio::ip::tcp;

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
    cout<< "starting getting bytes";
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
    cout<< "trying get message" << endl;
    return getFrameAscii(line, '\n');
}

bool ConnectionHandler::sendLine(std::string& line) {
    return sendFrameAscii(line, '\n');
}


bool ConnectionHandler::getFrameAscii(std::string& frame, char delimiter) {
    cout<< "1" << endl;
    char ch;
    // Stop when we encounter the null character.
    // Notice that the null character is not appended to the frame string.
    try {
        cout<< "2" << endl;

        short op_code = 0;
        if(!getBytes(&ch, 2))
        {
            cout<< "3" << endl;

            return false;
        }
        cout<< ch<< endl;
//        if(ch!='\0'){
            cout<< "4" << endl;

            op_code = bytesToShort(&ch);
            frame.append(2, ch);
            cout<<frame<<endl;
//        }
        if (op_code==13){
            cout<< "5" << endl;
            frame.append("ERROR ");
            cout<< frame << endl;
            if(!getBytes(&ch, 2))
            {
                cout<< "6" << endl;

                return false;
            }
            short message = bytesToShort(&ch);
            frame.append(std::to_string(message));
        } else{
            cout<< "7" << endl;

            frame.append("ACK ");
            if(!getBytes(&ch, 2))
            {
                cout<< "8" << endl;

                return false;
            }
            short message = bytesToShort(&ch);
            frame.append(std::to_string(message)+" ");
        }
        do{
            if(!getBytes(&ch, 1))
            {
                cout<< "9" << endl;

                return false;
            }
            if(ch!='\0'){
                cout<< "10" << endl;

                frame.append(1, ch);
            }
            cout<< "11" << endl;
        }while (delimiter != ch);
    } catch (std::exception& e) {
        std::cerr << "recv failed2 (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    cout<< "got " << frame << endl;
    return true;
}


bool ConnectionHandler::sendFrameAscii(const std::string& frame, char delimiter) {
    vector<char> msg = messageToByte(frame);
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

vector<char> ConnectionHandler::adminReg(const std::string& message) {
    char* username = "";
    char* password = "";
    int len = 2;
    int usernameLen = 0;
    int passwordLen = 0;
    bool usernamebool = true;
    for (int i = 9; i < message.length(); ++i) {
        if (message[i]!=' ') {
            if (usernamebool) {
                usernameLen++;
                username += message[i];
            }
            else {
                passwordLen++;
                password += message[i];
            }
        }
        else
            usernamebool = false;
    }
    len += usernameLen+passwordLen+2;   //2 start bytes, username and password len and 2 '0' bytes
    char output1[len];
    shortToBytes(1,output1);
    for (int i = 0; i < usernameLen; ++i) {
        output1[i+2] = username[i];
    }
    output1[usernameLen+2] = '\0';
    for (int i = 0; i < usernameLen; ++i) {
        output1[i+3+usernameLen] = username[i];
    }
    output1[len-1] = '\0';
    vector<char> output(0);
    for (int i = 0; i < len ; ++i) {
        output.push_back(output1[i]);
    }
    return output;
}

vector<char> ConnectionHandler::studentReg(const std::string& message) {
    char* username = "";
    char* password = "";
    int len = 2;
    int usernameLen = 0;
    int passwordLen = 0;
    bool usernamebool = true;
    for (int i = 9; i < message.length(); ++i) {
        if (message[i]!=' ') {
            if (usernamebool) {
                usernameLen++;
                username += message[i];
            }
            else {
                passwordLen++;
                password += message[i];
            }
        }
        else
            usernamebool = false;
    }
    len += usernameLen+passwordLen+2;   //2 start bytes, username and password len and 2 '0' bytes
    char output1[len];
    shortToBytes(2, output1);
    for (int i = 0; i < usernameLen; ++i) {
        output1[i + 2] = username[i];
    }
    output1[usernameLen + 2] = '\0';
    for (int i = 0; i < usernameLen; ++i) {
        output1[i + 3 + usernameLen] = username[i];
    }
    output1[len-1] = '\0';
    vector<char> output(0);
    for (int i = 0; i < len; ++i) {
        output.push_back(output1[i]);
    }
    return output;
}

vector<char> ConnectionHandler::login(const std::string& message) {
    char* username = "";
    char* password = "";
    int len = 2;
    int usernameLen = 0;
    int passwordLen = 0;
    bool usernamebool = true;
    for (int i = 9; i < message.length(); ++i) {
        if (message[i]!=' ') {
            if (usernamebool) {
                usernameLen++;
                username += message[i];
            }
            else {
                passwordLen++;
                password += message[i];
            }
        }
        else
            usernamebool = false;
    }
    len += usernameLen+passwordLen+2;   //2 start bytes, username and password len and 2 '0' bytes
    char output1[len];
    shortToBytes(3, output1);
    for (int i = 0; i < usernameLen; ++i) {
        output1[i + 2] = username[i];
    }
    output1[usernameLen + 2] = '\0';
    for (int i = 0; i < usernameLen; ++i) {
        output1[i + 3 + usernameLen] = username[i];
    }
    output1[len-1] = '\0';
    vector<char> output(0);
    for (int i = 0; i < len; ++i) {
        output[i] = output1[i];
    }
    return output;
}

vector<char> ConnectionHandler::logout(const std::string& message) {
    vector<char> output(0);
    char output1[2];
    shortToBytes(4,output1);
    output.push_back(output1[0]);
    output.push_back(output1[1]);
    return output;
}

vector<char> ConnectionHandler::courseReg(const std::string& message) {
    char* courseNumstr="";
    for (int i = 10; i <message.length(); ++i) {
        courseNumstr += message[i];
    }
    short courseNum = atoi(courseNumstr);
    char output1[4];
    shortToBytes(courseNum,output1);
    output1[2]=output1[0];
    output1[3]=output1[1];
    shortToBytes(5,output1);
    vector<char> output(0);
    for (int i = 0; i < 4; ++i) {
        output.push_back(output1[i]);
    }
    return output;
}

vector<char> ConnectionHandler::kdamCheck(const std::string& message) {
    char* courseNumstr="";
    for (int i = 10; i <message.length(); ++i) {
        courseNumstr += message[i];
    }
    short courseNum = atoi(courseNumstr);
    char output1[4];
    shortToBytes(courseNum,output1);
    output1[2]=output1[0];
    output1[3]=output1[1];
    shortToBytes(6,output1);
    vector<char> output(0);
    for (int i = 0; i < 4; ++i) {
        output.push_back(output1[i]);
    }
    return output;
}

vector<char> ConnectionHandler::courseStat(const std::string& message) {
    char* courseNumstr="";
    for (int i = 11; i <message.length(); ++i) {
        courseNumstr += message[i];
    }
    short courseNum = atoi(courseNumstr);
    char output1[4];
    shortToBytes(courseNum,output1);
    output1[2]=output1[0];
    output1[3]=output1[1];
    shortToBytes(7,output1);
    vector<char> output(0);
    for (int i = 0; i < 4; ++i) {
        output.push_back(output1[i]);
    }
    return output;
}

vector<char> ConnectionHandler::studentStat(const std::string& message) {
    char* username = "";
    int len = 2;
    for (int i = 12; i <message.length(); ++i) {
        username += message[i];
        len++;
    }
    len++;
    char output1[len];
    shortToBytes(8,output1);
    for (int i = 2; i < len-1; ++i) {
        output1[i] = username[i-2];
    }
    output1[len-1] = '\0';
    vector<char> output(0);
    for (int i = 0; i < len; ++i) {
        output.push_back(output1[i]);
    }
    return output;
}

vector<char> ConnectionHandler::isRegistered(const std::string& message) {
    char* courseNumstr="";
    for (int i = 13; i <message.length(); ++i) {
        courseNumstr += message[i];
    }
    short courseNum = atoi(courseNumstr);
    char output1[4];
    shortToBytes(courseNum,output1);
    output1[2]=output1[0];
    output1[3]=output1[1];
    shortToBytes(9,output1);
    vector<char> output(0);
    for (int i = 0; i < 4; ++i) {
        output.push_back(output1[i]);
    }
    return output;
}

vector<char> ConnectionHandler::unRegistered(const std::string& message) {
    char* courseNumstr="";
    for (int i = 11; i <message.length(); ++i) {
        courseNumstr += message[i];
    }
    short courseNum = atoi(courseNumstr);
    char output1[4];
    shortToBytes(courseNum,output1);
    output1[2]=output1[0];
    output1[3]=output1[1];
    shortToBytes(10,output1);
    vector<char> output(0);
    for (int i = 0; i < 4; ++i) {
        output.push_back(output1[i]);
    }
    return output;
}

vector<char> ConnectionHandler::myCourses(const std::string& message) {
    vector<char> output(0);
    char output1[2];
    shortToBytes(11,output1);
    output.push_back(output1[0]);
    output.push_back(output1[1]);
    return output;
}

vector<char> ConnectionHandler::whichOPCODE(string &op_code, const std::string& message) {
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
    else if (op_code == "UNREGISTERED")
        return unRegistered(message);
    else
        return myCourses(message);
}

vector<char> ConnectionHandler::messageToByte (const std::string& message){
    string op_code;
    for (int i = 0; i <message.length() ; ++i) {
        if (message[i] != ' ')
            op_code = op_code+message[i];
        else
            break;
    }
    return whichOPCODE(op_code,message);
}
