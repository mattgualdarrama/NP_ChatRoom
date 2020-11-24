#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <vector>
#include <thread>
#include <sstream>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

bool confirmConnection();

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
                string welcomeMessage = "Welcome to the chat server\r\n";
                send(client, welcomeMessage.c_str(), welcomeMessage.size() + 1, 0);

                // TODO: broadcast new connection to other users
                string newClient = "A new client user has joined!\r\n";
                for (int i = 0; i < master.fd_count; i++) {
                    SOCKET outSock = master.fd_array[i];
                    if (outSock != socListening && outSock != sock) {

                        send(outSock, newClient.c_str(), newClient.size() + 1, 0); //TODO Format, username, timestamp
                    }
                }
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
                    // Send message to other clients, NOT listening socket
                    for (int i = 0; i < master.fd_count; i++) {
                        SOCKET outSock = master.fd_array[i];
                        if (outSock != socListening && outSock != sock) {


                            ostringstream ss;
                            ss << "SOCKET #: " << sock << ": " << buff << "\r\n";
                            string msgOut = ss.str();

                            send(outSock, msgOut.c_str(), msgOut.size() + 1, 0); //TODO Format, username, timestamp
                        }
                    }
                }

            }
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