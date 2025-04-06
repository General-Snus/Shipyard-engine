#pragma once 
#include "NetMessage.h"
//Already included is
//NetworkedId Id;		UUID of the client
//TimePoint timePoint;	Time it was sent by the sender
struct HeartBeatData
{
	TimePoint serverTime;
	TimePoint timeSentByClient;
	float lastRoundTripTime;
	float bytePerSeconds;
};

class HeartBeatMessage: public NetMessage
{
public:
	HeartBeatMessage()
	{
		myType = type;
	}

	void SetMessage(const HeartBeatData &someData)
	{
		static_assert(NETMESSAGE_BUFFERSIZE > sizeof(someData));
		memcpy(&dataBuffer,&someData,sizeof(someData));
	}

	HeartBeatData ReadMessage() const
	{
		HeartBeatData data;
		memcpy(&data,&dataBuffer,sizeof(data));
		return data;
	}

	constexpr static eNetMessageType type = eNetMessageType::HearthBeat;
};
