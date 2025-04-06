#include "PersistentSystems.pch.h"
#include "../HeartBeatSystem.h"
#include "Networking/NetMessage/HeartBeatMessage.h"
#include "Networking/NetworkRunner.h"
#include "Tools\Utilities\Game\Timer.h"

#pragma optimize( "", off ) 

bool HeartBeatSystem::RecieveMessage(NetworkRunner& runner, const  RecievedMessage& msg)
{
	if (msg.message.myType != eNetMessageType::HearthBeat)
	{
		return false;
	}

	const auto pulse = std::bit_cast<HeartBeatMessage>(msg.message);
	const auto now = std::chrono::high_resolution_clock::now();
	const auto delta = TimerInstance.getDeltaTime();
	auto data = pulse.ReadMessage();

	this->downlinkPerSecond.Add(data.bytePerSeconds);

	if (runner.IsServer)
	{
		Remote* remote = runner.IdToRemote(pulse.GetId());

		if (remote == nullptr)
		{
			assert(false);
			return false;
		}

		if (!remote->hasConnectedOverUDP)
		{
			remote->udpAddress = msg.from;
			remote->hasConnectedOverUDP = true;
		}

		remote->lastHeartbeatTime = now;
		remote->roundTripBuffer.Add(data.lastRoundTripTime);

		HeartBeatMessage sendMessage;
		data.serverTime = now;
		data.bytePerSeconds = remote->sentDataPerFrame * delta;
		sendMessage.SetMessage(data);

		runner.SendTo(*remote, sendMessage, NetworkConnection::Protocol::UDP);
	}
	else
	{
		const auto serverToClientTripTime = (now - pulse.TimeSent());
		const auto clientToServerTripTime = (data.serverTime - data.timeSentByClient);
		const auto tripTime = serverToClientTripTime + clientToServerTripTime;

		float msRoundTrip = (float)std::chrono::duration_cast<std::chrono::microseconds>(tripTime).count() / 1000.f;
		roundTripBuffer.Add(msRoundTrip);
		lastRecievedHearthBeatMessage = now;
	}

	return true;
}

void HeartBeatSystem::UpdateClient(NetworkRunner& runner) const
{
	OPTICK_EVENT();
	const auto now = std::chrono::high_resolution_clock::now();
	HeartBeatMessage message;
	HeartBeatData data;
	data.bytePerSeconds = runner.uplinkRate();//Client sets it to sent, server set it to sent to client
	data.lastRoundTripTime = rtt();
	data.timeSentByClient = now;
	data.serverTime = {};
	message.SetMessage(data);
	runner.Send(message, NetworkConnection::Protocol::UDP);
}

void HeartBeatSystem::Update(NetworkRunner& runner)
{
	OPTICK_EVENT();

	if (!runner.IsServer)
	{
		UpdateClient(runner);
	}
	else
	{
		//Do throw away client here if respond time is to long
	}
}
#pragma optimize( "", on )


float  HeartBeatSystem::rtt() const
{
	return  float(roundTripBuffer.Sum()) / roundTripBuffer.Count();;
}

float HeartBeatSystem::downlinkRate() const
{
	return downlinkPerSecond.Average();
}
