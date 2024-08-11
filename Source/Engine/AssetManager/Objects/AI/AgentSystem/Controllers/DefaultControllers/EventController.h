#pragma once  
#include "Engine/AssetManager/Objects/AI/AgentSystem/AIEventManager.h"
#include "Engine/AssetManager/Objects/AI/NavMesh/NavMesh.h"
#include "../Controller.h"

class EventController : public Controller
{
public: 
	explicit EventController();
	bool Update(GameObject  aPosition) override;
	void Recieve(const AIEvent& aEvent) override;
private:
	bool pathToTarget;
	Vector3f end;
	NavMesh::Path myPath;
};
