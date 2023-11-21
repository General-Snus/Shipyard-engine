#include "PollingStation.h"

PollingStation::PollingStation(const Actor& aPlayer,const std::vector<Actor*>& aComputers) : player(&aPlayer)
{
	computers.insert(computers.end(),aComputers.begin(),aComputers.end());
}

Vector3f PollingStation::GetPlayerPosition() const
{
	return player->GetPosition();
}

std::vector<Vector3f> PollingStation::GetComputersPosition() const
{
	std::vector<Vector3f> out;
	for(size_t i = 0; i < computers.size(); i++)
	{
		out.push_back(computers[i]->GetPosition());
	}
	return out;
}
