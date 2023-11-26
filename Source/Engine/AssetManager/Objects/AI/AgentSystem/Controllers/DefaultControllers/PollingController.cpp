#include <AssetManager.pch.h>
#include "PollingController.h"
#include <Objects/AI/AgentSystem/PollingStation.h>

PollingController::PollingController(const PollingStation& aPollingStation) : pollingStation(aPollingStation)
{
	this->controllerType = eControllerType::polling;
}

bool PollingController::Update(GameObject aInput)
{
	aInput;
	return true;
}
