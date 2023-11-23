#include "AssetManager.pch.h"
#include "../cActor.h"   

cActor::cActor(const unsigned int anOwnerID) : Component(anOwnerID)
{myActor = new Actor();
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
		SteeringInput input;
		SteeringOutput output;
		//Update here


			//output = myActor->Update(input); //Velocity
		transform->Move(output.movement * deltatime);
		transform->Move(output.rotation * deltatime);
	}
}
Actor* cActor::GetActor()
{
	return myActor;
}
void cActor::Render()
{
}

Controller* cActor::GetController() const
{
	return myActor->GetController();
}

void cActor::SetController(Controller* aController)
{
	myActor->SetController(aController);
}