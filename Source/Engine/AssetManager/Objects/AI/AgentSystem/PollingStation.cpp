#include <AssetManager.pch.h>
#include "PollingStation.h"

PollingStation::PollingStation(const SY::UUID aPlayer,const std::vector<SY::UUID>& aComputers) : player(aPlayer)
{
	computers.insert(computers.end(),aComputers.begin(),aComputers.end());
}

Vector3f PollingStation::GetPlayerPosition() const
{
	if(auto* transform = GameObjectManager::GetInstance().TryGetComponent<Transform>(player))
	{
		return transform->GetPosition();
	}
	return Vector3f();
}

std::vector<Vector3f> PollingStation::GetComputersPosition() const
{
	std::vector<Vector3f> out;
	for(size_t i = 0; i < computers.size(); i++)
	{
		auto* transform = GameObjectManager::GetInstance().TryGetComponent<Transform>(computers[i]);
		if(transform)
		{
			out.push_back(transform->GetPosition());
		}
	}
	return out;
}
