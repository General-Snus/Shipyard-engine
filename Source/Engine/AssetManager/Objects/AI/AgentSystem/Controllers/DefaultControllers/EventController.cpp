#include <Engine/AssetManager/AssetManager.pch.h>
#include "EventController.h"

EventController::EventController()
{
	this->controllerType = eControllerType::event;
}

bool EventController::Update(GameObject input)
{
	auto* transform = input.TryGetComponent<Transform>();

	if(pathToTarget)
	{
		if(transform)
		{

			pathToTarget = false;
		}
		//myPath = GetNavMesh().PathFind(aPosition,end);
	}

	return true;
}

void EventController::Recieve(const AIEvent& aEvent)
{
	if(aEvent.type == eAIEvent::playerHacking)
	{
		myPath.myPath.clear();
		pathToTarget = true;
		end = aEvent.position;
	}
}
