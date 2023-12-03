#pragma once
#include "DefaultControllers/PollingController.h"
#include "../PollingStations/Target_PollingStation.h" 

class SeekerController : public PollingController
{
public:
	explicit SeekerController(MultipleTargets_PollingStation* aTargetsPollingStation,MultipleTargets_PollingStation* formationStation,GameObject componentCheck);
	bool Update(GameObject input) override;
	void Recieve(const AIEvent& aEvent) override;
private:
	PollingStation* FormationStation;
};

