#pragma once
#define DEFAULT_PORT 27015
#define DEFAULT_IP "127.0.0.1"
#include <string>
#include <unordered_map>
#include <WS2tcpip.h>

class Server
{
public:
	Server();
	~Server();

	void Setup();
	void Update();
	void Close();

private:
	SOCKET mySocket;

	std::unordered_map<unsigned char, std::string> myUsersnames;
	std::unordered_map<unsigned char, sockaddr_in> myUsersIPs;
	unsigned char myLatestId{1};
};

