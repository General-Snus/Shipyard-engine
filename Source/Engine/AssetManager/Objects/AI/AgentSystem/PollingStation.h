#pragma once
#include "Actor.h"


class PollingStation
{
public:
	PollingStation(const SY::UUID aPlayer,const std::vector<SY::UUID>& aComputers);
	Vector3f GetPlayerPosition() const;
	std::vector <Vector3f> GetComputersPosition() const;
private:
	const SY::UUID player;
	std::vector< SY::UUID> computers;
};