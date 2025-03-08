#pragma once
#include <string>  
#include <Tools/Utilities/TemplateHelpers.h>
#include <Tools/Utilities/System/SingletonTemplate.h>
#include <Engine/PersistentSystems/Networking/NetMessage/NetMessage.h>
#include "Engine/PersistentSystems/Networking/Client.h"
#include "Engine/PersistentSystems/Networking/NetworkStructs.h"  
#include <unordered_map>
#include <thread>
#include <mutex>
#include <span>
#include <Tools/Utilities/DataStructures/Queue.hpp>

#define Runner ServiceLocator::Instance().GetService<NetworkRunner>()
class NetworkRunner : public Singleton {
	friend class NetworkSettings;
	using MessageList = std::vector<std::pair<NetAddress,NetMessage>>;
	using NetworkMessagesMap = std::unordered_map<eNetMessageType,MessageList>; // vector has to be ordered by timestamp todo
public:
	NetworkRunner();
	NetworkRunner(const NetworkRunner&) = delete;
	NetworkRunner(const NetworkRunner&&) = delete;

	~NetworkRunner();
	NetworkConnection::Status StartSession(SessionConfiguration configuration);
	void update();
	void close();

	template<typename T> requires IsDerived<NetMessage,T>
	const MessageList& PollMessage();


	//If you are the server, message will be broadcasted
	//If you are the client, it will be sent to server and broadcasted by it
	bool Send(const NetMessage& message,NetworkConnection::Protocol protocol = NetworkConnection::Protocol::UDP);

public:
	bool Initialized{};
	bool HasInputAuthority{};
	bool HasStateAuthority{};
	bool IsServer{};

	static const int MAX_PLAYERS = 64;
private:

	bool Broadcast(const NetMessage& message,NetworkConnection::Protocol protocol); // Send to All
	bool Unicast(const NetMessage& message,Remote client,NetworkConnection::Protocol protocol); // Send to One
	bool Multicast(const NetMessage& message,std::span<Remote> client,NetworkConnection::Protocol protocol); // Send to Set

	void acceptNewClients(std::stop_token stop_token);
	void collectReceivedMessages(std::stop_token stop_token,NetworkConnection::Protocol protocol);
	void moveMessageMapToRead();

	//If you are a server, this connection represents the listening connection and it will create clients to handle the inbetween
	//If you are a client this will be you connection to the server
	NetworkConnection connection;

	// These function will always have state authority and will not be run if the server does not have it

	//MessageStorage
	NetworkMessagesMap  messagesMap;

	//Threaded messageStorage
	NetworkMessagesMap threadedMessageMap;
	std::mutex addMessageLock;

	//
	std::jthread receiveTCP;
	std::jthread receiveUDP;
	std::jthread acceptConnection;

	std::stop_token s_1;
	std::stop_token acceptConnectionStopToken;


	std::array<Remote,MAX_PLAYERS> clients;



	//Depricated
	std::unordered_map<unsigned char,std::string> usersNames;
	std::unordered_map<unsigned char,NetAddress> usersIPs;
	unsigned char                                  latestId{1};
};

template<typename T> requires IsDerived<NetMessage,T>
inline const NetworkRunner::MessageList& NetworkRunner::PollMessage() {
	return messagesMap[T::type];
}
