#include "PersistentSystems.pch.h"
#include "../HeartBeatSystem.h"
#include "Networking/NetMessage/HeartBeatMessage.h"
#include "Networking/NetworkRunner.h"

#pragma optimize( "", off ) 

void HeartBeatSystem::UpdateClient(NetworkRunner & runner)
{
	const auto now = std::chrono::high_resolution_clock::now();
	for(const auto& [index,message]: runner.PollMessage<HeartBeatMessage>() | std::ranges::views::enumerate)
	{
		const auto pulse = std::bit_cast<HeartBeatMessage>(message.message);
		auto data = pulse.ReadMessage();

		const auto serverToClientTripTime = (now - pulse.TimeSent()) ;
		const auto clientToServerTripTime = (data.serverTime - data.timeSentByClient) ;
		const auto tripTime = serverToClientTripTime+ clientToServerTripTime;
		usRoundTripTime = ((usRoundTripTime*index) + tripTime)/(index+1); //This is an average in case my iq is lower then it was on 22/03/2025 
		lastRecievedHearthBeatMessage = now;
	}

	HeartBeatMessage message;
	HeartBeatData data;
	data.sentMessagesToConnectionForLast10Seconds = 10; // todo
	data.lastRoundTripTime =usRoundTripTime;
	data.timeSentByClient = now;
	data.serverTime = {};
	message.SetMessage(data);
	runner.Send(message,NetworkConnection::Protocol::UDP);
}

void HeartBeatSystem::UpdateServer(NetworkRunner & runner)
{
	const auto now = std::chrono::high_resolution_clock::now();
	for(const auto& [index,message]: runner.PollMessage<HeartBeatMessage>() | std::ranges::views::enumerate)
	{
		const auto pulse = std::bit_cast<HeartBeatMessage>(message.message);
		auto data = pulse.ReadMessage();
		Remote* t = runner.IdToRemote(pulse.GetId());

		if(t == nullptr) {
			assert(false);
			continue;
		}

		if(!t->hasConnectedOverUDP)
		{
			t->udpAddress = message.from;
			t->hasConnectedOverUDP = true;
		}

		t->lastHeartbeatTime = now;
		t->rtt = data.lastRoundTripTime;

		HeartBeatMessage sendMessage;
		data.serverTime = now;
		data.sentMessagesToConnectionForLast10Seconds = 10; // todo
		sendMessage.SetMessage(data);

		runner.Send(sendMessage,NetworkConnection::Protocol::UDP);
	}

}

void HeartBeatSystem::Update(NetworkRunner & runner)
{
	if(runner.IsServer) {
		UpdateServer(runner);
	} else{
		UpdateClient(runner);
	}
}
#pragma optimize( "", on )


Duration  HeartBeatSystem::rtt() const
{
	return usRoundTripTime;
}
