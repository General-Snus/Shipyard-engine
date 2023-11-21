#pragma once
#include "Controller.h"
#include "../NavMesh/NavMesh.h"
class EventController : public Controller
{
public: 
	explicit EventController();
	Vector3f Update(const Vector3f aPosition) override;
	void Recieve(const AIEvent& aEvent) override;
private:
	bool pathToTarget;
	Vector3f end;
	NavMesh::Path myPath;
};