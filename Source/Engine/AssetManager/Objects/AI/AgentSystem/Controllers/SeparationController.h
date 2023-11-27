#pragma once 
#include "DefaultControllers/PollingController.h"


#define decayCoefficient 5.0f
#define threshold 5.5f
#define maxAcceleration 100.0f

class SeparationController : public PollingController
{
public:
	explicit SeparationController(MultipleTargets_PollingStation* station, GameObject componentCheck);
	bool Update(GameObject input) override;
	void Recieve(const AIEvent& aEvent) override;
private: 
};

