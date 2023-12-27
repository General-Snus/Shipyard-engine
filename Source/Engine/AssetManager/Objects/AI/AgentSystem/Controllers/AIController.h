#pragma once
#include "../PollingStations/Target_PollingStation.h" 
#include "DefaultControllers/PollingController.h"

class AIController : public PollingController
{
public:
	explicit AIController(MultipleTargets_PollingStation* aTargetsPollingStation,MultipleTargets_PollingStation* formationStation);
	bool Update(GameObject input) override;
	void Recieve(const AIEvent& aEvent) override;
	bool ComponentRequirement(GameObject input) override;
private:
	PollingStation* FormationStation;
};

