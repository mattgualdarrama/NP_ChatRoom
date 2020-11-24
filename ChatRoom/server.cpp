#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <vector>
#include <thread>

#pragma comment (lib, "ws2_32.lib")

//test 
using namespace std;

bool confirmConnection();

int main() {



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