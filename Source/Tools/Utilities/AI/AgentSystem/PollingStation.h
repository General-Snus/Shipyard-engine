#pragma once
#include "Actor.h"


class PollingStation
{
public:
	PollingStation(const Actor& aPlayer,const std::vector<Actor*>& aComputers);
	Vector3f GetPlayerPosition() const;
	std::vector <Vector3f> GetComputersPosition() const;
private:
	const Actor* player;
	std::vector<Actor*> computers;
};