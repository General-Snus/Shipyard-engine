#include "PersistentSystems.pch.h"
#include "../HeartBeatSystem.h"
#include "Networking/NetMessage/HeartBeatMessage.h"
#include "Networking/NetworkRunner.h"

#pragma optimize( "", off ) 

void HeartBeatSystem::UpdateClient(NetworkRunner & runner)
{
	OPTICK_EVENT();

	const auto now = std::chrono::high_resolution_clock::now();
	for(const auto& [index,message]: runner.PollMessage<HeartBeatMessage>() | std::ranges::views::enumerate)
	{
		const auto pulse = std::bit_cast<HeartBeatMessage>(message.message);
		auto data = pulse.ReadMessage();

		const auto serverToClientTripTime = (now - pulse.TimeSent()) ;
		const auto clientToServerTripTime = (data.serverTime - data.timeSentByClient) ;
		const auto tripTime = serverToClientTripTime+ clientToServerTripTime;

		float msRoundTrip = (float)std::chrono::duration_cast<std::chrono::microseconds>(tripTime).count()/1000.f;
		roundTripBuffer.Add(msRoundTrip); 
 		lastRecievedHearthBeatMessage = now;
	}

	HeartBeatMessage message;
	HeartBeatData data;
	data.sentMessagesToConnectionForLast10Seconds = 10; // todo
	data.lastRoundTripTime = rtt();
	data.timeSentByClient = now;
	data.serverTime = {};
	message.SetMessage(data);
	runner.Send(message,NetworkConnection::Protocol::UDP);
}

void HeartBeatSystem::UpdateServer(NetworkRunner & runner)
{
	OPTICK_EVENT();

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
 		t->roundTripBuffer.Add(data.lastRoundTripTime);

		HeartBeatMessage sendMessage;
		data.serverTime = now;
		data.sentMessagesToConnectionForLast10Seconds = 10; // todo
		sendMessage.SetMessage(data);

		runner.Send(sendMessage,NetworkConnection::Protocol::UDP);
	}

}

void HeartBeatSystem::Update(NetworkRunner & runner)
{
	OPTICK_EVENT();

	if(runner.IsServer) {
		UpdateServer(runner);
	} else{
		UpdateClient(runner);
	}
}
#pragma optimize( "", on )


float  HeartBeatSystem::rtt() const
{
	return  float(roundTripBuffer.Sum())/roundTripBuffer.Count();;
}
