#pragma once
#include "Controller.h"
#include "../NavMesh/NavMesh.h"
class EventController : public Controller
{
public: 
	explicit EventController();
	SteeringOutput Update(const SteeringInput& aPosition) override;
	void Recieve(const AIEvent& aEvent) override;
private:
	bool pathToTarget;
	Vector3f end;
	NavMesh::Path myPath;
};