#include "Engine/AssetManager/AssetManager.pch.h"
#include "Engine/AssetManager/Objects/AI/AgentSystem/AIEventManager.h"

#include "Engine/AssetManager/Objects/AI/AgentSystem/Controllers/Controller.h"

AIEventManager::AIEventManager()
{ 
}

void AIEventManager::RegisterListener(const eAIEvent aEventType,Controller* aController)
{
	eventListener[aEventType].push_back(aController);
}

void AIEventManager::Update()
{
	/*
	* Example code for a custom hacking event
	if(CheckForHackingEvent())
	{
		AIEvent event{};
		event.position = player->GetPosition();
		event.type = eAIEvent::playerHacking;
		DispatchEvent(event);
	}
	*/
}
/*
* Example code for a custom hacking event
bool AIEventManager::CheckForHackingEvent()
{
	constexpr float hackingDistance = 100.0f;
	int a = 0;
	for(const auto& i : computers)
	{
		if(a != pathToward && (player->GetPosition() - i->GetPosition()).Length() < hackingDistance)
		{
			pathToward = a;
			return true;
		}
		a++;
	}
	return false;
}
*/

void AIEventManager::DispatchEvent(const AIEvent& aEvent)
{
	for(auto& i : this->eventListener[aEvent.type])
	{
		i->Recieve(aEvent);
	}
}
