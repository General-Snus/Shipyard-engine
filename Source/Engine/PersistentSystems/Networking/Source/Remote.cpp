#include "PersistentSystems.pch.h" 
#include "../Remote.h"

#include <Tools/Logging/Logging.h>
#include <Tools/Utilities/Math.hpp>
#include "Engine/PersistentSystems/Networking/NetMessage/NetMessage.h"
#include <Networking/NetMessage/StringMessages.h>
#include "Networking/NetworkRunner.h"

void Remote::Close() { // todo
	OPTICK_EVENT();
	receiveTCP.request_stop();
	remoteConnection.Close();
	isConnected = false;
	hasConnectedOverUDP= false;
	nickname= "RemoteNickname";
	id = {}; 
	lastRecievedMessageTime = {};
	lastHeartbeatTime = {};
	roundTrip = {}; 
	Consume(); 
}

const std::vector<Remote::RemoteRecievedMessage>& Remote::Read() {
	std::scoped_lock remoteLock(messageMutex);
	return messages;
}

void Remote::Consume() {
	std::scoped_lock remoteLock(messageMutex);
	messages.clear();
}

void Remote::TryUDPConnection(NetAddress serverAddress)
{
	serverAddress;
}

NetAddress Remote::AddressByProtocol(NetworkConnection::Protocol protocol)
{
	switch(protocol)
	{
	case NetworkConnection::Protocol::UDP:
		return udpAddress;
		break;
	case NetworkConnection::Protocol::TCP:
		return remoteConnection.Address();
		break;
	default:
		std::unreachable();
		break;
	}
}

float Remote::rtt() const
{
	return float(roundTripBuffer.Sum()) / (float)roundTripBuffer.Count();
}

void Remote::collectReceivedMessages(std::stop_token stop_token) {
	NetMessage incomingMessage;
	NetAddress recievedFromAddress;

	OPTICK_THREAD("Remote::collectReceivedMessages");
	while(!stop_token.stop_requested()) {
		if(auto error = remoteConnection.ReceiveTCP(&incomingMessage,&recievedFromAddress) == ReceiveSuccessful) {
			std::scoped_lock lock(messageMutex);
			readDataPerFrame += sizeof(incomingMessage);
			//bit discusting but here we go
			if(incomingMessage.myType == eNetMessageType::NewConnection)
			{
				HandshakeMessage message = std::bit_cast<HandshakeMessage>(incomingMessage);
				nickname = message.ReadMessage();
				id = message.GetId();
			}


			lastRecievedMessageTime = std::chrono::high_resolution_clock::now();
			RemoteRecievedMessage recv{
				incomingMessage,
				recievedFromAddress
			};

			messages.emplace_back(recv);
		} else {
			isConnected = false;
			this->Close();
			LOGGER.Err(std::format("Remote connection encountered errors and will shut down. {}",NetworkHelpers::GetWSAErrorString(error)));
		}

	}
}
