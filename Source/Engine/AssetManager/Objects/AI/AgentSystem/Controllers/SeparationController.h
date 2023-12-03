#pragma once 
#include "DefaultControllers/PollingController.h"



class SeparationController : public PollingController
{
public:
	explicit SeparationController(MultipleTargets_PollingStation* station, GameObject componentCheck);
	bool Update(GameObject input) override; 
	void Recieve(const AIEvent& aEvent) override;
private: 
};

