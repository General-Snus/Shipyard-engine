#include "AssetManager.pch.h"
#include "../cActor.h" 

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
		myActor->SetPosition(myActor->GetPosition());
	}
}

void cActor::Render()
{
}
