#pragma once
#include "DefaultControllers/PollingController.h" 
class WanderController : public PollingController
{
public:
	explicit WanderController(const PollingStation& aPollingStation,GameObject componentCheck);
	bool Update(GameObject input) override;
	void Recieve(const AIEvent& aEvent) override;
private:

};

