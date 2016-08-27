#pragma once
#ifndef HSOCK_H
#define HSOCK_H
#include <string>
#include <WinSock2.h>

using namespace std;

// constants
const string HTTP_BREAK = "\r\n";
const string USERAGENT = "Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/40.0.2214.85 Safari/537.36";
const int BUFFER_SIZE = 1024;

// convert std::string to char *
char * StringToChar(string str);

// Python's raw_input() in C++
string raw_input(char * message);
string raw_input(string message);

// Create Raw HTTP Get Request
char * formHttpGet(string subsite, string dnsName);

// A Python tuple replacement
struct HAddr {
	const char * ipv4;
	int port;
	char * hostIpv4;
};

// Python tuple of URL Subsite and DNS Name
struct HUrl {
	string subsite;
	string dnsName;
	HUrl(string _sub, string _dns) {
		subsite = _sub;
		dnsName = _dns;
	}
};

// Create addresses with ease
HAddr create_addr(const char * ipv4, int port);

// Basic Python socket object
class HSock {
	SOCKET sock;

	public:
		HSock(int sFamily, int sType, int sProtocol);
		~HSock();

		void setSock(int sFamily, int sType, int sProtocol) {
			this->sock = socket(sFamily, sType, sProtocol);
		}
		SOCKET getSock() {
			return this->sock;
		}

		int hconnect(HAddr addr);
		int hclose();

		int hsend(char * data);
		string hrecv();
};

// Send & Receive HTTP Get Request
string simpleGet(HSock sock, char * url);

// Get Url DNS Name and Subsite
HUrl getDnsAndSubsite(char * url);

#endif