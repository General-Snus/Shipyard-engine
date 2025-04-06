#pragma once 
#include "NetMessage/NetMessage.h" 
#include "Tools/Utilities/DataStructures/CircularBuffer.h"
#include "Tools/Utilities/DataStructures/MathStructs.h"

class NetworkRunner;
class HeartBeatSystem
{
public:
	HeartBeatSystem() = default;
	void Update(NetworkRunner& runner);
	float rtt() const;
	float downlinkRate() const;
	bool RecieveMessage(NetworkRunner& runner,const RecievedMessage& msg);
private:
	void UpdateClient(NetworkRunner& runner) const;
	Avg<float, 10000> downlinkPerSecond; // The server includes sent data per second to the client so it can compared to its recieved data 
	TimePoint lastRecievedHearthBeatMessage; // if client, also if server for no real reason
	CircularBuffer<float,10> roundTripBuffer;
};

