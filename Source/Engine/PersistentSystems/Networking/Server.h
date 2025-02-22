#pragma once
#define DEFAULT_PORT 27015
#define LOCALHOST "127.0.0.1"
#include <string>
#include <unordered_map>
#include <Engine/PersistentSystems/Networking/NetMessage/NetMessage.h>
 
class NetMessage;
struct NetworkMessage;
struct NetworkConnection;
struct NetAddress;
struct SessionConfiguration;
struct sockaddr;

//All value in here should be network byte order or whatever that means
struct NetAddress {
public:
	NetAddress();

	unsigned long address{};
	unsigned short port{}; 

	sockaddr asAddress() const;
};

struct NetworkConnection {
public :
	enum class Status {
		initialized = 1,
		failed
	};


	Status Initialize(int socketType = 2, int socketProtocol = 0,unsigned short bindType = 2); //sock dgram / 0 / af inet, 

	bool Receive(NetMessage* const message, NetAddress* recivedFrom, const int bufferSize = 512, const float timeout = 0.f);
	bool Send(const NetMessage& message, const NetAddress& sendTo);
	
	bool Close();

private:
	Status status;
	NetAddress homeAddress;
	unsigned long long Socket;
};

struct NetworkMessage {

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

class NetworkRunner
{
public:
	NetworkRunner();
	~NetworkRunner();

	HRESULT StartSession(SessionConfiguration configuration);

	void setup();
	void update();
	void close();

public:
	bool HasInputAuthority;
	bool HasStateAuthority;

private:
	void networkTransformUpdate();
	NetworkConnection connection;

	std::unordered_map<unsigned char, std::string> usersNames;
	std::unordered_map<unsigned char,NetAddress> usersIPs;
	unsigned char                                  latestId{1};
};
