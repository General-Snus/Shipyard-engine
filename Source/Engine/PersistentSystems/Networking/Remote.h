#pragma once
#include <iostream>
#include <thread> 
#include <mutex> 
#include <vector> 
#include "Engine/PersistentSystems/Networking/NetMessage/NetMessage.h"
#include "Engine/PersistentSystems/Networking/NetworkStructs.h" 
#include <Tools/Utilities/uuidv4/uuid_v4.h>
#include <Tools/Utilities/DataStructures/CircularBuffer.h>
#pragma comment(lib, "Ws2_32.lib")
#undef max

struct Remote { 
	friend class NetworkRunner;
	friend class HeartBeatSystem;
	friend class NetworkSettings;
	struct RemoteRecievedMessage {
		NetMessage message;
		NetAddress from;
	};;
	
	NetAddress udpAddress;
	NetworkConnection remoteConnection;
	bool isConnected;

	const std::vector<RemoteRecievedMessage>& Read();
	void Close();
	void Consume();
	void TryUDPConnection(NetAddress serverAddress); 
	NetAddress AddressByProtocol(NetworkConnection::Protocol protocol);
	float rtt() const;
private:
	bool hasConnectedOverUDP = false;
	NetworkedId id;
	std::string nickname;
	TimePoint lastRecievedMessageTime;
	TimePoint lastHeartbeatTime;
	Duration roundTrip;
	CircularBuffer<float,15> roundTripBuffer;
	std::vector<RemoteRecievedMessage> messages;
	std::mutex messageMutex;
	std::jthread receiveTCP;

	void collectReceivedMessages(std::stop_token stop_token);
}; 