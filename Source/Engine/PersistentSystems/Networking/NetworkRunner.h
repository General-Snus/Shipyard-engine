#pragma once
#include <Tools/Utilities/TemplateHelpers.h>
#include <Tools/Utilities/System/SingletonTemplate.h>
#include <Engine/PersistentSystems/Networking/NetMessage/NetMessage.h>
#include "Engine/PersistentSystems/Networking/Remote.h"
#include "Engine/PersistentSystems/Networking/NetworkStructs.h"
#include "Engine/PersistentSystems/Networking/HeartBeatSystem.h"
#include <Tools/Utilities/DataStructures/Queue.hpp>
#include "ReplicationLayer.h"

#include <unordered_map>
#include <thread>
#include <string>
#include <mutex> 
#include <ranges> 
#include "Engine\AssetManager\ComponentSystem\Components\Network\NetworkSync.h"
#include "Tools\Utilities\TemplateHelpers.h"

//This entire thing is fucked, you should handle messages directly not per frame update and construct commands for the engine in all cases but thats a lot of time
#define Runner ServiceLocator::Instance().GetService<NetworkRunner>()
class NetworkRunner : public Singleton {
	friend class NetworkSettings;
	friend class ReplicationLayer;
	friend class HeartBeatSystem;
	friend class NetworkObject;

public:
	using MessageList = std::vector<RecievedMessage>;
	using NetworkMessagesMap = std::unordered_map<eNetMessageType, MessageList>; // vector has to be ordered by timestamp todo

	NetworkRunner();
	NetworkRunner(const NetworkRunner&) = delete;
	NetworkRunner(const NetworkRunner&&) = delete;
	~NetworkRunner();

	NetworkConnection::Status StartSession(SessionConfiguration configuration);

	void ZeroDataRates();

	void Update();
	void ProcessIncoming(const eNetMessageType& type, RecievedMessage& incomingMessage);
	void Close();

	template<typename T> requires IsDerived<NetMessage, T>
	const MessageList& PollMessage();

	template<typename T> requires IsDerived<NetMessage, T>
	bool ReadMessages(NetworkRunner::MessageList&) const;

	//If you are the server, message will be broadcasted
	//If you are the client, it will be sent to server and broadcasted by it
	//Will set the id of the message to the runner to ensure consistency, this layer is aware of id, connection is not
	bool Send(NetMessage& message, NetworkConnection::Protocol protocol);
	bool SendTo(const Remote& remote, NetMessage& message, NetworkConnection::Protocol protocol) const;
	bool SendTo(const NetworkedId& remote, NetMessage& message, NetworkConnection::Protocol protocol) const;

	bool HasConnection(NetworkedId id);
	std::string NameOfConnection(NetworkedId id);
	std::optional<const Remote&> IdToRemote(NetworkedId id) const;
	Remote* IdToRemote(NetworkedId id);
 
	ServerTimePoint serverTime() const;

	auto ConnectedRemote() const
	{
		if (!IsServer)
		{
			throw std::runtime_error("Clients do not have access to active remotes");
		}

		return remoteConnections | std::views::filter([](const Remote& r) { return r.isConnected; });
	}

	auto AllPlayers() const
	{
		if (!IsServer)
		{
			throw std::runtime_error("Clients do not have access to active remotes");
		}
		auto retArg = remoteConnections
			| std::views::filter([](const Remote& r) { return r.isConnected && Runner.layer.ContainsObject(r.areaOfInterest.owner); })
			| std::views::transform([](const Remote& r) -> const auto&
		{
			NetworkObject* obj = nullptr;
			Runner.layer.TryFetchNetworkObject(r.areaOfInterest.owner, obj);
			return *obj;
		});

		return retArg;
	}

	float uplinkRate(); // per second
	float downlinkRate();
	float downlinkRate(int remoteIndex);
	float uplinkRate(int remoteIndex);
	// per second
public:
	ReplicationLayer layer;
	HeartBeatSystem heartBeatSystem;
	bool Initialized{};
	bool HasInputAuthority{};
	bool HasStateAuthority{};
	bool IsServer{};
	std::string runnerName = "Username"; // "Your userHandle"

	bool udpConnectionInitialized{};
	NetAddress serverUdpConnection;

	static const int MAX_PLAYERS = 64;
private:
	bool registerObject(const NetworkObject& object);
	bool RegisterPlayer(const NetworkObject& object) const;
	bool unRegisterObject(const NetworkObject& object);


	bool Broadcast(NetMessage& message, NetworkConnection::Protocol protocol) const; // Send to All
	bool Unicast(NetMessage& message, Remote client, NetworkConnection::Protocol protocol)  const; // Send to One

	bool Multicast(NetMessage& message, InputRange auto&& client, NetworkConnection::Protocol protocol) const; // Send to Set

	void acceptNewClients(std::stop_token stop_token);
	void collectReceivedMessages(std::stop_token stop_token, NetworkedId recieversOwnId, NetworkConnection::Protocol protocol);
	void moveMessageMapToRead();

	NetworkedId runnerID;

	//If you are a server, this connection represents the listening connection and it will create clients to handle the inbetween
	//If you are a client this will be you connection to the server
	NetworkConnection connection;
	std::atomic<int> readDataPerFrame;
	std::atomic<int> sentDataPerFrame;
	int cachedSentDataPerFrame;

	//[Server]
	std::array<Remote, MAX_PLAYERS> remoteConnections;

	// These function will always have state authority and will not be run if the server does not have it 
	//MessageStorage
	NetworkMessagesMap  messagesMap;

	//Threaded messageStorage
	NetworkMessagesMap threadedMessageMap;
	std::mutex addMessageLock;

	std::jthread receiveTCP;
	std::jthread receiveUDP;
	std::stop_token s_1;
	std::stop_token acceptConnectionStopToken;
};

template<typename T> requires IsDerived<NetMessage, T>
inline const NetworkRunner::MessageList& NetworkRunner::PollMessage()
{
	return messagesMap[T::type];
}

template<typename T> requires IsDerived<NetMessage, T>
inline bool NetworkRunner::ReadMessages(NetworkRunner::MessageList& read) const
{
	if (messagesMap.contains(T::type))
	{
		read = messagesMap.at(T::type);
		return true;
	}
	else
	{
		return false;
	}
}


inline bool NetworkRunner::Multicast(NetMessage& message, InputRange auto&& rangeOfClient, NetworkConnection::Protocol protocol) const
{
	OPTICK_EVENT();
	bool allSucceded = true;
	for (const Remote& client : rangeOfClient)
	{
		if (!client.isConnected) { continue; }
		allSucceded &= SendTo(client, message, protocol);
	}

	return allSucceded;
}
