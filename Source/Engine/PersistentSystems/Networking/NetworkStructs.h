#pragma once
#define DEFAULT_PORT 27015
#define LOCALHOST "127.0.0.1"
class NetMessage;  
struct NetworkConnection;
struct NetAddress;
struct SessionConfiguration;
struct sockaddr;

//All value in here should be network byte order or whatever that means
struct NetAddress {
public:
	NetAddress();
	NetAddress(const std::string& Ip,unsigned short port);

	unsigned long address{};
	unsigned short port{};

	sockaddr asAddress() const;
};

struct NetworkConnection {
public:
	enum class Status {
		failed = -1,
		initialized = 1
	};

	//Will attempt to bind to socket and start server
	Status StartAsServer(NetAddress serverAddress,int socketType = 2,int socketProtocol = 0,unsigned short bindType = 2); //sock dgram / 0 / af inet, 
	//Will attempt to connect to socket, if fail will call startServer
	Status AutoHostOrClient(NetAddress serverAddress, int socketType = 2,int socketProtocol = 0,unsigned short bindType = 2); //sock dgram / 0 / af inet, 
	//Will attempt to connect to socket
	Status StartAsClient(NetAddress serverAddress,int socketType = 2,int socketProtocol = 0,unsigned short bindType = 2); //sock dgram / 0 / af inet, 

	bool Receive(NetMessage* message,NetAddress* recivedFrom,const int bufferSize = 512,const float timeout = 0.f) const;
	bool Send(const NetMessage& message) const;

	bool Close() const;
private:
	Status status;
	NetAddress connectedTo;
	unsigned long long Socket;
};

struct SessionConfiguration {
	enum class GameMode {
		Single = 1,
		Shared,
		Server,
		Host,
		Client,
		AutoHostOrClient,
		count
	} gameMode;
};