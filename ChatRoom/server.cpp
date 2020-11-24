#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <sstream>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

bool validateCommand(string);
bool confirmConnection();
void broadcastMessage(string, fd_set&, SOCKET, SOCKET);

int main() {
    if (!confirmConnection()) {
        cerr << "Winsock error";
        return 1;
    }

    // Create a listening socket
    SOCKET socListening = socket(AF_INET, SOCK_STREAM, 0);
    if (socListening == INVALID_SOCKET) {
        cerr << "Cannot create listneing socket.";
        return 1;
    }

    // Bind IP address and port to socket
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    hint.sin_addr.S_un.S_addr = INADDR_ANY;

    bind(socListening, (sockaddr*)&hint, sizeof(hint));

    // Tell server to listen on socListen
    listen(socListening, SOMAXCONN);

    // Define
    fd_set master;
    FD_ZERO(&master); //ensure FD is clear

    FD_SET(socListening, &master);

    //main event loop
    while (1) {
        fd_set masterCopy = master;

        int socketCount = select(0, &masterCopy, NULL, NULL, NULL); //sockets will be stored in masterCopy

        for (int i = 0; i < socketCount; i++) {
            SOCKET sock = masterCopy.fd_array[i];

            if (sock == socListening) {
                // Accept new connection
                SOCKET client = accept(socListening, NULL, NULL);

                // Add new connection to master file set
                FD_SET(client, &master);
                
                // Send a welcome message
                string welcomeMessage = "Welcome to the chat server";
                broadcastMessage(welcomeMessage, master, socListening, sock);

                string topicMessage = "Current topic: None";

                // TODO: broadcast new connection to other users
                string newClient = "A new client user has joined!";
                broadcastMessage(newClient, master, socListening, sock);
            }
            else {
                char buff[8192];
                ZeroMemory(buff, sizeof(buff)); //fill buffer with 0s

                // Receiving message
                int readBytes = recv(sock, buff, sizeof(buff), 0);
                if (readBytes <= 0) {
                    // Connection closed, drop client
                    closesocket(sock);
                    FD_CLR(sock, &master);
                }
                else {

                    //TODO:  Check for commands
                    // Commands will always start with an '!' and be exactly 4 alpha letters.  eg !help
                    string command = "";
                    if (buff[0] == '!') {
                        for (int i = 1; i < 5; i++)
                            command = command + buff[i];

                        if (validateCommand(command)) {

                        }
                        else {
                            string invldCommand = "Invalid command\r\n";
                            send(sock, invldCommand.c_str(), invldCommand.size() + 1, 0); //send message to the user with invalid command, do not broadcast to all users
                            continue;
                        }
                    }


                       
                    // Send message to other clients, NOT listening socket
                    for (int i = 0; i < master.fd_count; i++) {
                        SOCKET outSock = master.fd_array[i];
                        if (outSock != socListening && outSock != sock) {

                            //TODO usernames
                            ostringstream ss;
                            ss << "SOCKET # " << sock << ": " << buff;
                            string msgOut = ss.str();
                            broadcastMessage(msgOut, master, socListening, sock);

                        }
                    }
                }

            }
        }
    } 

}

bool validateCommand(string c) {
    return false;
}
/*
* A function to broadcast a message to all clients currently connected
*   message: message to send
*   master: the file descriptor set
*   listeningSocket: active listening socket
*   currentSocket: 
*   
*/
void broadcastMessage(string message, fd_set &master, SOCKET listeningSocket, SOCKET currentSocket) {
    for (int i = 0; i < master.fd_count; i++) {
        SOCKET outSock = master.fd_array[i];

        message = message + "\r\n";  //Add a new line to each message

        if (outSock != listeningSocket && outSock != currentSocket) {
            send(outSock, message.c_str(), message.size() + 1, 0);
        }
    }
}

bool confirmConnection() {
    // Make sure that we can connect:
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD(1, 1);  // A macro to create a word with low order and high order bytes as specified.
    err = WSAStartup(wVersionRequested, &wsaData);

    if (err != 0) {

        // Tell the user that we couldn't find a useable winsock.dll. */
        cerr << "Could not find useable DLL" << endl;
        return false;
    }
    // Confirm that the Windows Sockets DLL supports version 1.1.
    // Note that if the DLL supports versions greater than 1.1 in addition 
    //  to 1.1, it will still return1.1 in wVersion since that is the version we/
    // requested.  Note the use of macros.

    if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1) {

        // Tell the user that we couldn't find a useable winsock DLL. 
        cerr << "Could not find useable DLL" << endl;
        WSACleanup();  // Terminates the use of the library. 
        return false;
    }

    return true;
}