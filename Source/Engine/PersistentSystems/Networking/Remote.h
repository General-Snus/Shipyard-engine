#pragma once
#include <iostream>
#include <thread> 
#include <mutex> 
#include <vector> 
#include "Engine/PersistentSystems/Networking/NetMessage/NetMessage.h"
#include "Engine/PersistentSystems/Networking/NetworkStructs.h" 
#include <Tools/Utilities/uuidv4/uuid_v4.h>
#pragma comment(lib, "Ws2_32.lib")
#undef max

struct Remote { 
	friend class NetworkRunner;
	friend class NetworkSettings;
	struct RemoteRecievedMessage {
		NetMessage message;
		NetAddress from;
	};;
	
	NetworkConnection remoteConnection;
	bool isConnected;

	const std::vector<RemoteRecievedMessage>& Read();
	void Close();
	void Consume();
private:
	NetworkedId id;
	std::string nickname;
	std::chrono::time_point<std::chrono::system_clock> lastRecievedMessageTime;
	std::vector<RemoteRecievedMessage> messages;
	std::mutex messageMutex;
	std::jthread receiveTCP;

	void collectReceivedMessages(std::stop_token stop_token);
}; 