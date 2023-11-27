#pragma once
#include "../Controller.h" 
#include <Engine/AssetManager/Objects/AI/NavMesh/NavMesh.h>

class PollingStation;

class PollingController : public Controller
{
public:
	explicit PollingController(PollingStation* aPollingStation);
	bool Update(GameObject input) override;

protected:
	PollingStation* pollingStation; 
};