/*
Name: HSock (HTTP. Socket)
Date: 8/20/2016
Description: A Python-like socket class
*/

#include "HSock.h"
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#define _WINSOCK_DEPRECATED_NO_WARNINGS

using namespace std;

// convert string to char array
char * StringToChar(string str) {
	char * char_str = new char[str.length() + 1];
	strcpy(char_str, str.c_str());
	return char_str;
}

// Python's raw_input in C++
string share_input() {
	string input;
	getline(cin, input);
	return input;
}
string raw_input(char * message) {
	cout << message;
	return share_input();
}
string raw_input(string message) {
	cout << message;
	return share_input();
}

// Constructor
HSock::HSock(int sFamily, int sType, int sProtocol) {
	setSock(sFamily, sType, sProtocol);
}

// Destructor
HSock::~HSock() {
	hclose();
}

// Address creation
HAddr create_addr(const char *ipv4, int port) {
	HAddr address;
	address.ipv4 = ipv4;
	address.port = port;
	return address;
}

/* String Splitting
credit: http://stackoverflow.com/questions/236129/split-a-string-in-c/236803#236803
*/
void _split(const string &s, char delim, vector<string> &elems) {
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
}

vector<string> split(const string&s, char delim) {
	vector<string> elems;
	_split(s, delim, elems);
	return elems;
}

// Create basic HTTP GET Request
char * formHttpGet(string subsite, string dnsName) {
	string HTTP_GET;

	HTTP_GET = "GET " + subsite + " HTTP/1.1" + HTTP_BREAK;
	HTTP_GET += "Accept-Encoding identity" + HTTP_BREAK;
	HTTP_GET += "Host: " + dnsName + HTTP_BREAK;
	HTTP_GET += "Connection: close" + HTTP_BREAK;
	HTTP_GET += "User-Agent: " + USERAGENT + HTTP_BREAK;
	HTTP_GET += HTTP_BREAK;

	// convert to char array
	return StringToChar(HTTP_GET);
}

HUrl getDnsAndSubsite(char * url) {
	// get url parts
	vector<string> parts = split(url, '/');

	// remove empty characters
	parts.erase(remove(parts.begin(), parts.end(),
		string("")), parts.end());

	// get dns name and subsite of url
	int count = 0;
	string dnsName;
	string subsite;
	for (auto & part : parts) {
		switch (count) {
			case 0: { /*This is the http protocol*/ }
			case 1: {
				dnsName = part;
				break;
			}
			default: {
				subsite += "/" + part;
				break;
			}
		}
		count++;
	}
	return HUrl(subsite, dnsName);
}

// Simple HTTP Get request
string simpleGet(HSock sock, char *url) {

	HUrl urlParts = getDnsAndSubsite(url);

	// build HTTP Request
	char *getRequest = formHttpGet(urlParts.subsite, urlParts.dnsName);

	// send HTTP Request
	cout << "Receiving data..." << endl;
	sock.hsend(getRequest);

	// receive response
	return sock.hrecv();
}

// Connect to address
int HSock::hconnect(HAddr addr) {

	// Setup network address
	struct hostent *host;
	try {
		// get host address raw
		host = gethostbyname(addr.ipv4);
		// get host address text
		addr.hostIpv4 = inet_ntoa(*((struct in_addr *) host->h_addr_list[0]));
	}
	catch (exception &ex) {
		cout << "Failed to get socket address of " << addr.ipv4 << endl;
		return 1;
	}

	// Create socket address
	SOCKADDR_IN SockAddr;
	SockAddr.sin_port   = htons(addr.port);
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);

	// Connect to address via socket
	cout << "Connecting to " << addr.hostIpv4 << endl;
	if (connect(getSock(), (SOCKADDR*)(&SockAddr), sizeof(SockAddr)) == 0) {
		cout << "[" << addr.hostIpv4 << "] connection successful!" << endl;
		return 0;
	}
	else {
		cout << "[" << addr.hostIpv4 << "] connection Failed!" << endl;
		return 1;
	}
}

/* Close existing socket */
int HSock::hclose() {
	return closesocket(getSock());
}

/* Send data to socket */
int HSock::hsend(char *message) {
	return send(getSock(), message, strlen(message), 0);
}

/* Recv data from socket */
string HSock::hrecv() {
	// variables
	string response;
	int responseSize = BUFFER_SIZE;

	// chunk data from socket
	while (true) {
		char buffer[BUFFER_SIZE];
		responseSize = recv(getSock(), (char *)&buffer, BUFFER_SIZE, 0);

		if (responseSize == -1)
			return string("Failed to receive data!");
		else if (responseSize == 0)
			break;
		else
			response += string(buffer).substr(0, responseSize);
	}

	// return completed response
	return response;
}