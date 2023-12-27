#include "AssetManager.pch.h"
#include "../cActor.h"   

cActor::cActor(const SY::UUID anOwnerID) : Component(anOwnerID)
{
}

void cActor::Init()
{

}

void cActor::Update()
{
	OPTICK_EVENT();
	if(auto* transform = TryGetComponent<Transform>()) //Actors will was accepted, do checks around here if allowed to move external forces such as transform lock
	{
		if(controller)
		{
			controller->Update(GetGameObject()); //Im giving the controller all the rights over the object here
		}
	}
}
void cActor::Render()
{
}


void cActor::SetController(Controller* aController)
{
	controller = aController;
	controller->ComponentRequirement(GetGameObject());
}