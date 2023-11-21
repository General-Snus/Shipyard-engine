#include "PollingController.h"
#include "PollingStation.h" 

PollingController::PollingController(const PollingStation& aPollingStation) : pollingStation(aPollingStation)
{
	this->controllerType = eControllerType::polling;
}

Vector3f PollingController::Update(const  Vector3f aPosition)
{
	constexpr float hackingDistance = 1.0f;
	for(const auto& i : pollingStation.GetComputersPosition())
	{
		if( (pollingStation.GetPlayerPosition()-i).Length() < hackingDistance)
		{
			return (i - aPosition);
		}
	}
	return Vector3f();
}
