#pragma once 
#include "NetMessage\NetMessage.h"
#include "Tools\Utilities\DataStructures\CircularBuffer.h"

class NetworkRunner;
class HeartBeatSystem
{
public:
	HeartBeatSystem() = default;
	void Update(NetworkRunner & runner);
	float rtt() const;
private:
	void UpdateClient(NetworkRunner & runner);
	void UpdateServer(NetworkRunner & runner);

	TimePoint lastRecievedHearthBeatMessage; // if client, also if server for no real reason
 	CircularBuffer<float,10> roundTripBuffer;
};

