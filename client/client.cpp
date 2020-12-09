#include <iostream>
#include <string>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;
#pragma comment (lib, "ws2_32.lib")

string IPADDRESS = "127.0.0.1";
int PORT = 54000;



int main() {

	string userName;
	int userPassword, serverPasswrod;

	cout << "Enter username:\t";	cin >> userName;
	cout << "Enter password:\t";	cin >> userPassword;
	cout << "Enter server pass:\t";	cin >> serverPasswrod;

}

