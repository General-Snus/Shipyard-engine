#pragma once 
#include "DefaultControllers/PollingController.h"


class MultipleTargets_PollingStation;

class SeparationController : public PollingController, public Reflectable<SeparationController>
{
public:
	reflectable(SeparationController);
	explicit SeparationController(MultipleTargets_PollingStation* station, GameObject componentCheck);
	bool Update(GameObject input) override; 
	void Recieve(const AIEvent& aEvent) override;
private: 
};

REFL_AUTO(type(SeparationController))
