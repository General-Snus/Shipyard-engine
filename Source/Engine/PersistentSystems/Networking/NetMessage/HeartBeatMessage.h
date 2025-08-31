#pragma once 
#include "NetMessage.h"
#include <cstring>  

struct HeartBeatData
{
	TimePoint serverTime;
	TimePoint timeSentByClient;
	float lastRoundTripTime;
	float bytePerSeconds;
	Networking::AreaOfInterest aoi;
};

class HeartBeatMessage : public NetMessage
{
public:
	HeartBeatMessage()
	{
		myType = type;
	}

	void SetMessage(const HeartBeatData& someData)
	{
		static_assert(NETMESSAGE_BUFFERSIZE > sizeof(HeartBeatData), "Buffer too small for HeartBeatData");
		std::memcpy(&dataBuffer, &someData, sizeof(HeartBeatData));
	}

	HeartBeatData ReadMessage() const
	{
		HeartBeatData data;
		std::memcpy(&data, &dataBuffer, sizeof(HeartBeatData));
		return data;
	}

	constexpr static eNetMessageType type = eNetMessageType::HearthBeat;
};
