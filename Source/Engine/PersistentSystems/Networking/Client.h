#pragma once
#include <iostream>
#include <thread> 
#include <mutex> 
#include <vector> 
#include "Engine/PersistentSystems/Networking/NetMessage/NetMessage.h"
#include "Engine/PersistentSystems/Networking/NetworkStructs.h" 
#pragma comment(lib, "Ws2_32.lib")
#undef max

struct Remote { 
	friend class NetworkRunner;
	NetworkConnection remoteConnection;
	bool isConnected;

	const std::vector<NetMessage>& Read();
	void Close();
	void Consume();
private:
	std::vector<NetMessage> messages;
	std::mutex messageMutex;
	std::jthread receiveTCP;

	void collectReceivedMessages(std::stop_token stop_token);
}; 