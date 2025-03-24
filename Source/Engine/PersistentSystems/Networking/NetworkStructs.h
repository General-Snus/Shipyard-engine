#pragma once
#include <Tools/Utilities/TemplateHelpers.h>
#include <Tools/Utilities/uuidv4/uuid.h>
#define DEFAULT_PORT 27015
#define LOCALHOST "127.0.0.1"
class NetMessage;
struct NetworkConnection;
struct NetAddress;
struct SessionConfiguration;
struct sockaddr;
struct sockaddr_in;

struct NetworkedId {
	static NetworkedId Generate();

	NetworkedId();
	explicit NetworkedId(uuid::v4::UUID id): id(id) {};
	uuid::v4::UUID id;

	//std::strong_ordering operator<=>(const NetworkedId& other) const {
	//	return id <=> other.id;
	//}
	//
	// Custom equality operator
	bool operator==(const NetworkedId& other) const {
		return id == other.id;
	};

	// Custom assignment operator
	NetworkedId& operator=(const uuid::v4::UUID& uuid) {
		id = uuid;
		return *this;
	}
};

static_assert(std::is_trivially_copyable<NetworkedId>::value,"NetworkedId must be trivially copyable");
static_assert(std::is_trivially_copyable<uuid::v4::UUID>::value,"UUIDv4::UUID be trivially copyable");

namespace std {
template <>
struct hash<NetworkedId> {
	std::size_t operator()(const NetworkedId& networkedId) const {
		return std::hash<uuid::v4::UUID>{}(networkedId.id);
	}
};
}

//All value in here should be network byte order or whatever that means
struct NetAddress {
	enum class AddressFamily: unsigned short {
		ipv4 = 2, // AF_INET,
		ipv6 = 23 // AF_INET6
	};

	NetAddress();
	NetAddress(const std::string& Ip,unsigned short port,AddressFamily family = AddressFamily::ipv4);
	std::string IPStr() const;
	sockaddr_in as_sockaddr_in() const;
	sockaddr as_sockaddr() const;

	unsigned long address{};
	unsigned short port{};
	AddressFamily family = AddressFamily::ipv4;
};
using NetworkSocket = unsigned long long;
constexpr static int ReceiveSuccessful = 0;

struct NetworkConnection {
public:
	enum class Status: int {
		initializedAsServer = 0b0000'0001,
		initializedAsClient = 0b0000'0010,

		//Failure states
		incorrectIP = 0b0000'0100,

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
	//FOR SOME REASON SUCCESS HAVE TO BE ZERO,A TRUE RETURN MEANS ERROR AND THE ERROR CODE IS THE RETURN
	int ReceiveTCP(NetMessage* message,NetAddress* recivedFrom,const int bufferSize = 512,bool blocking = true,const float timeout = -1.f) const;


	bool Accept(NetworkConnection* returningConnection) const;

	static bool UDP(NetAddress toAddress,NetworkSocket throughSocket,NetMessage& message);
	static bool TCP(NetworkSocket throughSocket,NetMessage& message);

	bool SendUDP(NetAddress toAddress,NetMessage& message) const;
	bool SendTCP(NetMessage& message) const;

	Status GetStatus() const;
	NetAddress Address() const;
	bool Close();
private:
	Status status;
	NetAddress tcpConnectionAddress;
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

#define CASE(x) case (x): return #x;
std::string GetWSAErrorString(int error);

}

