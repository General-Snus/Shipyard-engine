#include "AssetManager.pch.h"
#include "../PollingController.h" 

PollingController::PollingController(PollingStation* aPollingStation) : pollingStation(aPollingStation)
{
	this->controllerType = eControllerType::polling;
}

bool PollingController::Update(GameObject aInput)
{
	aInput;
	return true;
}
