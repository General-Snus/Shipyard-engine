#pragma once
#include <string>  
#include <Tools/Utilities/TemplateHelpers.h>
#include <Tools/Utilities/System/SingletonTemplate.h>
#include <Engine/PersistentSystems/Networking/NetMessage/NetMessage.h>
#include "Engine/PersistentSystems/Networking/Client.h"
#include "Engine/PersistentSystems/Networking/NetworkStructs.h"  
#include <unordered_map>

#define Runner ServiceLocator::Instance().GetService<NetworkRunner>()
class NetworkRunner : public Singleton {
public:
	NetworkRunner();
	~NetworkRunner();
	NetworkConnection::Status StartSession(SessionConfiguration configuration); 
	void update();
	void close();

	template<typename T> requires IsDerived<NetMessage,T>
	const std::vector<NetMessage*>& ReadIncoming();
	 
public:
	bool HasInputAuthority{};
	bool HasStateAuthority{};
	bool IsServer{};

private:
	void networkTransformUpdate();
	NetworkConnection connection;
	std::vector<Client> clients;
	// These function will always have state authority and will not be run if the server does not have it
	std::unordered_map<eNetMessageType,std::vector<NetMessage*>> netMessageTypeHandling;

	//Consume for each tick all after tick
	//When a client returns anything, add it to queue and server will then  let the messagehandling consume it

	std::unordered_map<unsigned char,std::string> usersNames;
	std::unordered_map<unsigned char,NetAddress> usersIPs;
	unsigned char                                  latestId{1};
};

template<typename T> requires IsDerived<NetMessage,T>
inline const std::vector<NetMessage*>& NetworkRunner::ReadIncoming() {
	return netMessageTypeHandling[T::type];
}
