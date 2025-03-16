#pragma once
#include <string>  
#include <Tools/Utilities/TemplateHelpers.h>
#include <Tools/Utilities/System/SingletonTemplate.h>
#include <Engine/PersistentSystems/Networking/NetMessage/NetMessage.h>
#include "Engine/PersistentSystems/Networking/Remote.h"
#include "Engine/PersistentSystems/Networking/NetworkStructs.h"  
#include <unordered_map>
#include <thread>
#include <mutex>
#include <span>
#include <Tools/Utilities/DataStructures/Queue.hpp>

#define Runner ServiceLocator::Instance().GetService<NetworkRunner>()
class NetworkRunner : public Singleton {
	friend class NetworkSettings;
	friend class ReplicationLayer;

public:
	struct RecievedMessage {
		NetMessage message;
		NetAddress from;
		NetworkedId idFrom; 
		NetworkConnection::Protocol protocol;
	};
	using MessageList = std::vector<RecievedMessage>;
	using NetworkMessagesMap = std::unordered_map<eNetMessageType,MessageList>; // vector has to be ordered by timestamp todo

	NetworkRunner();
	NetworkRunner(const NetworkRunner&) = delete;
	NetworkRunner(const NetworkRunner&&) = delete;

	~NetworkRunner();
	NetworkConnection::Status StartSession(SessionConfiguration configuration);
	void Update();
	void ProcessIncoming(const eNetMessageType& type,NetworkRunner::RecievedMessage& incomingMessage);
	void Close();

	template<typename T> requires IsDerived<NetMessage,T>
	const MessageList& PollMessage();

	//If you are the server, message will be broadcasted
	//If you are the client, it will be sent to server and broadcasted by it
	//Will set the id of the message to the runner to ensure consistency, this layer is aware of id, connection is not
	bool Send(NetMessage& message,NetworkConnection::Protocol protocol);
	bool HasConnection(NetworkedId id);
	std::string NameOfConnection(NetworkedId id);
	Remote* IdToRemote(NetworkedId id);
public:
	ReplicationLayer layer;
	bool Initialized{};
	bool HasInputAuthority{};
	bool HasStateAuthority{};
	bool IsServer{};
	std::string runnerName = "Username"; // "Your userHandle"

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
	NetworkedId runnerID;
	NetworkConnection connection;
	std::array<Remote,MAX_PLAYERS> remoteConnections;

	// These function will always have state authority and will not be run if the server does not have it 
	//MessageStorage
	NetworkMessagesMap  messagesMap;

	//Threaded messageStorage
	NetworkMessagesMap threadedMessageMap;
	std::mutex addMessageLock;

	//
	std::jthread receiveTCP;
	std::jthread receiveUDP; 
	std::stop_token s_1;
	std::stop_token acceptConnectionStopToken;
};

template<typename T> requires IsDerived<NetMessage,T>
inline const NetworkRunner::MessageList& NetworkRunner::PollMessage() {
	return messagesMap[T::type];
}
