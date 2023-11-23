#include "PollingController.h"
#include "PollingStation.h" 

PollingController::PollingController(const PollingStation& aPollingStation) : pollingStation(aPollingStation)
{
	this->controllerType = eControllerType::polling;
}

SteeringOutput PollingController::Update(const  SteeringInput& aInput)
{
	SteeringOutput output;
	constexpr float hackingDistance = 1.0f;
	for(const auto& i : pollingStation.GetComputersPosition())
	{
		if((pollingStation.GetPlayerPosition() - i).Length() < hackingDistance)
		{
			output.movement= (i - aInput.position);
		}
	}
	return output;
}
