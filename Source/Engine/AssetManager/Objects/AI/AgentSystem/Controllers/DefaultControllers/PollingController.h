#pragma once
#include "../Controller.h" 
#include <Engine/AssetManager/Objects/AI/NavMesh/NavMesh.h>

class PollingStation;

class PollingController : public Controller
{
public:
	MYLIB_REFLECTABLE()
	explicit PollingController(PollingStation* aPollingStation);
	bool Update(GameObject input) override;

protected:
	PollingStation* pollingStation; 
};
REFL_AUTO(type(PollingController))