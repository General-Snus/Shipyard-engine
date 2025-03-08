#pragma once
#include <Tools/Utilities/TemplateHelpers.h>
#define DEFAULT_PORT 27015
#define LOCALHOST "127.0.0.1"
class NetMessage;
struct NetworkConnection;
struct NetAddress;
struct SessionConfiguration;
struct sockaddr;
struct sockaddr_in;

//All value in here should be network byte order or whatever that means
struct NetAddress {
	enum class AddressFamily : unsigned short {
		ipv4 = 2, // AF_INET,
		ipv6 = 23 // AF_INET6
	};

	NetAddress();
	NetAddress(const std::string& Ip,unsigned short port,AddressFamily family = AddressFamily::ipv4);
	std::string IPStr();
	sockaddr_in as_sockaddr_in() const;
	sockaddr as_sockaddr() const;

	unsigned long address{};
	unsigned short port{};
	AddressFamily family = AddressFamily::ipv4;
};
using NetworkSocket = unsigned long long;

struct NetworkConnection {
public:
	enum class Status : int {
		initializedAsServer = 1 << 0,
		initializedAsClient = 1 << 1,
		//Failure states
		incorrectIP = 1 << 2,

		//Summasion states
		initialized = initializedAsServer | initializedAsClient,
		failed = incorrectIP,
	};
	enum class Protocol {
		UDP = 1,TCP = 2
	};

	//Will attempt to bind to socket and start server
	Status StartAsServer(NetAddress serverAddress,SessionConfiguration cfg);
	//Will attempt to connect to socket, if fail will call startServer
	Status AutoHostOrClient(NetAddress serverAddress,SessionConfiguration cfg);
	//Will attempt to connect to socket ----- BAD NAMING CLIENT SOCKET IS A DEFINED NAME THIS IS NOT IT TODO MF
	Status StartAsClient(NetAddress serverAddress,SessionConfiguration cfg);
	
	//TcP OnlY EFoRcE HoW todo
	Status StartAsRemoteConnection(NetAddress serverAddress,NetworkSocket socket);

	NetworkConnection::Status OpenSockets(SessionConfiguration cfg);

	bool ReceiveUDP(NetMessage* message,NetAddress* recivedFrom,const int bufferSize = 512,bool blocking = true,const float timeout = -1.f) const;
	bool ReceiveTCP(NetMessage* message,NetAddress* recivedFrom,const int bufferSize = 512,bool blocking = true,const float timeout = -1.f) const;
	bool Accept(NetworkConnection* returningConnection) const;
	
	static bool UDP(NetAddress toAddress, NetworkSocket throughSocket,const NetMessage& message);
	static bool TCP(NetworkSocket throughSocket,const NetMessage& message);
 
	bool Send(const NetMessage& message,Protocol protocol = Protocol::UDP) const;
	bool Send(const NetMessage& message,NetAddress target,Protocol protocol = Protocol::UDP) const;
	bool SendUDP(NetAddress toAddress,const NetMessage& message) const;

	Status GetStatus() const;
	NetAddress Address() const;
	bool Close();
private:
	Status status;
	NetAddress connectedTo;
	NetworkSocket TCPSocket;
	NetworkSocket UDPSocket;
};

ENABLE_ENUM_BITWISE_OPERATORS(NetworkConnection::Status);

struct SessionConfiguration {
	enum class GameMode {
		Single = 0,
		Shared,
		Server,
		Host,
		Client,
		AutoHostOrClient
		//count Dont write out count, we have magic_enum for this
	};

	enum class HostType {
		Local,
		LAN,
		WAN,
		Steam
	};

	GameMode gameMode;
	HostType hostType;
	NetAddress address = NetAddress();
};

namespace NetworkHelpers {

	struct SocketSettings {
		int receiveBufferSize;
		int sendBufferSize;
		bool reuseAddress;
		bool isBlocking;
		bool tcpNoDelay;
		int maxSegmentSize;
		bool keepAlive;
		int keepIdle;
		int keepInterval;
		int keepCount;
	};

	//i really cant be bothered to write this shit myself, gpt it is
	SocketSettings GetSocketSettings(unsigned long long socket);
}

