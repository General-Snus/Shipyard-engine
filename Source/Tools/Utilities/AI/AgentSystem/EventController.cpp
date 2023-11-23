#include "EventController.h"

EventController::EventController()
{
	this->controllerType = eControllerType::event;
}

SteeringOutput EventController::Update(const SteeringInput& input)
{
	SteeringOutput output;
	if(pathToTarget)
	{
		pathToTarget = false;
		//myPath = GetNavMesh().PathFind(aPosition,end);
	}
	if(myPath.myPath.size())
	{
		if((input.position - myPath.myPath[0]).Length() < nodeDistance)
		{
			myPath.myPath.erase(myPath.myPath.begin());
		}
		if(myPath.myPath.size())
		{
			output.movement = (myPath.myPath[0] - input.position);
		}
		else
		{
			output.movement = Vector3f();
		}
	}
	return output;
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
