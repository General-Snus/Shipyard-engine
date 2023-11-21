#include "PollingController.h"
#include "PollingStation.h" 

PollingController::PollingController(const PollingStation& aPollingStation) : pollingStation(aPollingStation)
{
	this->controllerType = eControllerType::polling;
}

Vector3f PollingController::Update(const  Vector3f aPosition)
{
	constexpr float hackingDistance = 100.0f;
	int a = 0;
	bool isHacking = false;
	for(const auto& i : pollingStation.GetComputersPosition())
	{
		if( (pollingStation.GetPlayerPosition()-i).Length() < hackingDistance)
		{
			isHacking = true;
			if(a != pathToward)
			{
				pathToward = a;
				myPath.myPath.clear();
				end = i;
				//myPath = GetNavMesh().PathFind(aPosition,end);
			}
		}
		a++;
	}
	if(!isHacking)
	{
		pathToward = -1;
		myPath.myPath.clear();
	}
	if(myPath.myPath.size())
	{
		if((aPosition-myPath.myPath[0]).Length() < nodeDistance)
		{
			myPath.myPath.erase(myPath.myPath.begin());
		}
		if(myPath.myPath.size())
		{
			return (myPath.myPath[0] - aPosition);
		}
		else
		{
			return Vector3f();
		}
	}
	return Vector3f();
}
