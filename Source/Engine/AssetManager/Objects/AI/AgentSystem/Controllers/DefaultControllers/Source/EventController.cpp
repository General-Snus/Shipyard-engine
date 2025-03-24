#include "AssetManager.pch.h"

#include "../EventController.h" 
#include "Engine/AssetManager/Objects/AI/AgentSystem/AIEventManager.h"
#include "Engine/AssetManager/Objects/AI/AgentSystem/Controllers/Controller.h"

EventController::EventController()
{
	this->controllerType = eControllerType::event;
}

bool EventController::Update(GameObject input)
{
	const auto* transform = input.TryGetComponent<Transform>();

	if (pathToTarget)
	{
		if (transform)
		{

			pathToTarget = false;
		}
		//myPath = GetNavMesh().PathFind(aPosition,end);
	}

	return true;
}

void EventController::Recieve(const AIEvent& aEvent)
{
	if (aEvent.type == eAIEvent::playerHacking)
	{
		myPath.myPath.clear();
		pathToTarget = true;
		end = aEvent.position;
	}
}
