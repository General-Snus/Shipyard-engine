#include "AssetManager.pch.h"
#include "../cActor.h"  
#include <Tools/Utilities/AI/AgentSystem/Actor.h>

cActor::cActor(const unsigned int anOwnerID) : Component(anOwnerID)
{
	myActor = new Actor(); //TODO Actor pool? alla mina vänner hatar raw pekare
}

void cActor::Init()
{
	
}

void cActor::Update()
{
	auto* transform = TryGetComponent<Transform>();
	float deltatime = Timer::GetInstance().GetDeltaTime();
	
	if(transform) //Actors will was accepted, do checks around here if allowed to move external forces such as transform lock
	{
		transform->Move(myActor->Update(deltatime));
		myActor->SetPosition(transform->GetPosition());
	}
}

void cActor::Render()
{
}

Actor* cActor::GetActor()
{
	return myActor;
}