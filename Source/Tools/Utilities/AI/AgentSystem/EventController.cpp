#include "EventController.h"

EventController::EventController()
{
	this->controllerType = eControllerType::event;
}

Vector3f EventController::Update(const Vector3f aPosition)
{
	if(pathToTarget)
	{
		pathToTarget = false;
		//myPath = GetNavMesh().PathFind(aPosition,end);
	}
	if(myPath.myPath.size())
	{
		if((aPosition - myPath.myPath[0]).Length() < nodeDistance)
		{
			myPath.myPath.erase(myPath.myPath.begin());
		}
		if(myPath.myPath.size())
		{
			return (myPath.myPath[0] - aPosition);
		}
		else
		{
			return  Vector3f();
		}
	}
	return  Vector3f();
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
