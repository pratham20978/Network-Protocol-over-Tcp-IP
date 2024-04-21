#include <iostream>
#include <boost/asio.hpp>
#include "Header_Files.h"
#include "CR_Message.h"
#include "CR_TSQueue.h"
#include <fstream>
#include <openssl/sha.h>
//#include <sstream>
#include <iomanip>
#include <cstdlib>

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;




//----------------------------------------------------------------------------------------------------------------------

// Function to set terminal attributes to turn off echoing
void turnOffEcho() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

// Function to restore terminal attributes
void restoreTerminal() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}


//------------------------------------------------------------------------------------------------------------------------


// Hash functio to store hash valued password

// Function to compute SHA-256 hash of a string
std::string computeSHA256Hash(const std::string& input) {
    // Create stringstream to store hashed output
    std::stringstream hashedOutput;

    // Compute hash using std::hash function
    std::hash<std::string> hasher;
    size_t hashValue = hasher(input);

    // Convert hash value to hexadecimal string
    hashedOutput << std::hex << std::setw(64) << std::setfill('0') << hashValue;

    // Return hashed output as string
    return hashedOutput.str();
}

//---------------------------------------------------------------------------------------------



// Function to save creadentials in file
void Save_Creadentials_To_File(const string& UserName, const string& Password){

    // File to store creadentials
    ofstream file("Creadentials.txt");

    if(file.is_open()){
        file << UserName <<endl;
        file << Password <<endl;
        
        file.close();

        cout<<"Creadentials Succesfully Saved to file."<<endl;
    }else{
        cerr<<"Unable to open file for writing."<<endl;
    }
}

bool Authenticate(const string& username, const string& password){

    ifstream file("Creadentials.txt");
    if(file.is_open()){
        string storedUsername, storedPassword;
        while(file >> storedUsername >> storedPassword){
            if(storedUsername == username && storedPassword == password){
                file.close();
                return true;
            }
        }
        file.close();
    }
    return false;
}

bool Check_If_User_Exists(const string& username, const string& password){
    ifstream file("Creadentials.txt");
    if(file.is_open()){
        string storedUsername, storedPassword;
        while(file >> storedUsername >> storedPassword){
            if(username == storedUsername){
                cout<<"User alreadu exists. Please login."<<endl;
                return true;
            }
        }
        file.close();
    }
    return false;
}

void Register(){
    string username,password;
    cout<<"Enter Username: ";
    cin>>username;
    cout<<"Enter Password: ";
    turnOffEcho();
    cin>>password;
    restoreTerminal();

    string hashValue = computeSHA256Hash(password);

    if(!Check_If_User_Exists(username,hashValue)){
        Save_Creadentials_To_File(username,hashValue);
        system("sudo chmod 400 Creadentials.txt");
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------


olc::net::Message getData(tcp::socket& socket){

    olc::net::Message msg;
    boost::asio::streambuf buf;
    read_until(socket, buf, "\n");
    istream is(&buf);
    string data;
    getline(is,data);
    data.pop_back();    
    msg.DeserilizeMsg(data);

    return msg;
}


void SendData(tcp::socket& socket, const olc::net::Message& msg){

    string message = msg.SerilizeMsg();
    write(socket, buffer(message + "\n"));
}



//-----------------------------------------------------------------------------------------------------------------------




int main(){

    // Connect to Other User
    string UserIP="192.168.1.9";
    uint16_t port = 9999;
    

    bool loggedIn = false; // Flag to trace login status;

    string UserName;


    // Manage Login Page
    while(!loggedIn){
        int choice;
        cout<<"1. Login"<<endl;
        cout<<"2. Register"<<endl;
        // cout<<"3. Change Username $ Password"<<endl;
        cout<<"Enter you choice: "<<endl;

        cin>>choice;


        switch (choice) {
            case 1: {
                string username, password;
                cout << "Enter username: ";
                cin >> username;
                cout<<endl;
                cout << "Enter password: ";
                turnOffEcho();
                cin >> password;
                restoreTerminal();
                cout<<endl;
                string hashValue = computeSHA256Hash(password);
                if (Authenticate(username, hashValue)) {
                    cout << "Login successful!" << endl;
                    cout<<endl;
                    loggedIn = true;
                    UserName = username;
                } else {
                    cout << "Invalid username or password. Please try again." << endl;
                }
                break;
            }
            case 2:
                Register();
                system("sudo chmod 511 Header_Files.h");
                system("sudo chmod 511 CR_Message.h");
                system("sudo chmod 511 CR_TSQueue.h");
                system("sudo chmod 111 Private_Chat_Room.cpp");
                break;
            default:
                cout << "Invalid choice." << endl;
        }
    }

    system("clear");

    // Clear the input buffer
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    io_service io_service;
    thread p_thread;
    olc::net::TSQueue<olc::net::Message> qIn;
    tcp::socket socket(io_service);
    tcp::acceptor acceptor_server(io_service, tcp::endpoint(tcp::v4(), port));

    cout<<"Connecting......."<<endl;

    try{
        socket.connect(tcp::endpoint(address::from_string(UserIP), port));
        cout<<"[Connected]"<<endl;
    }catch(const exception &ec){
        // wait for connection
        socket.close();
        acceptor_server.accept(socket);
        cout<<"[Connected]"<<endl;
    }


    p_thread = thread([&](){

        // pushing incoming message in Queue
        while(true){
            qIn.push(getData(socket));
        }

    });


    while(true){
        while(!qIn.empty()){ 
            olc::net::Message msgresponse;
            msgresponse = qIn.get();
            qIn.pop();
            cout<<"["<<msgresponse.READ_HDR()<<"]: ";
            cout<<msgresponse.READ_MSG()<<endl;         
        }

        string reply;
        cout<<"["<<UserName<<"]: ";
        getline(cin,reply);
        if(reply == "exit"){
            break;
            socket.close();
        }
        if(reply!= ""){
            olc::net::Message msg(UserName, reply);
            SendData(socket, msg);
        }
    }


    return 0;
}
