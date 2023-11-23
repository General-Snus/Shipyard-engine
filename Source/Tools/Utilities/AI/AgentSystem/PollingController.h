#pragma once
#include "Controller.h" 
#include "../NavMesh/NavMesh.h"

class PollingStation;

class PollingController : public Controller
{
public:
	explicit PollingController(const PollingStation& aPollingStation);
	SteeringOutput Update(const SteeringInput& aPosition) override;

private:
	const PollingStation& pollingStation;
	int pathToward;
	Vector3f end;
	NavMesh::Path myPath;
};