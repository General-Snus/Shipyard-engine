#include <AssetManager.pch.h>
#include "PollingController.h"
#include "../PollingStation.h"

PollingController::PollingController(const PollingStation& aPollingStation) : pollingStation(aPollingStation)
{
	this->controllerType = eControllerType::polling;
}

SteeringOutput PollingController::Update(GameObject aInput)
{
	SteeringOutput output; 
	auto* transform = aInput.TryGetComponent<Transform>();
	constexpr float hackingDistance = 1.0f;

	for(const auto& i : pollingStation.GetComputersPosition())
	{
		if((pollingStation.GetPlayerPosition() - i).Length() < hackingDistance)
		{
			output.movement = (i - transform->GetPosition()).GetNormalized();
		}
	}
	return output;
}
