#include "AssetManager.pch.h"
#include "../SeekerController.h"
#include <Tools/Utilities/LinearAlgebra/Quaternions.hpp>

SeekerController::SeekerController(Target_PollingStation* aPollingStation,GameObject componentCheck) : PollingController(aPollingStation)
{
	//Check for required components
	if(!componentCheck.TryGetComponent<cPhysics_Kinematic>())
	{
		auto& phy = componentCheck.AddComponent<cPhysics_Kinematic>();
		phy.localVelocity = true;
	}
}

bool SeekerController::Update(GameObject input)
{
	auto* physicsComponent = input.TryGetComponent<cPhysics_Kinematic>();
	auto& transform = input.GetComponent<Transform>(); // You can use Get directly if you are sure it will exist, its faster and force safe code

	auto targetPos = reinterpret_cast<Target_PollingStation*>(pollingStation)->GetTargetPosition();
	auto direction = (targetPos - transform.GetPosition()).GetNormalized();
	auto fwd = transform.GetForward(); 

	auto angles = Quaternionf::RotationFromTo(fwd,direction).GetEulerAngles() * RAD_TO_DEG;

	//Scrub data that can accidentaly enter the system from my bad non quaternion math
	angles.x = 0;
	angles.z = 0;

	physicsComponent->ph_Angular_velocity = angles;
	physicsComponent->ph_velocity = 4.5f * GlobalFwd;  

	{//Special circumstance limit area
		const float mapsize = 50.0f;
		Vector3f position = transform.GetPosition();//varför funkar inte mod som i matte :,( hur var det detta som tog längst tid av allt
		position = {
			(Mod<float>((position.x),mapsize)),
			position.y,
			(Mod<float>((position.z),mapsize))
		};
		transform.SetPosition(position);
		return true;
	}
}

void SeekerController::Recieve(const AIEvent& aEvent)
{
	aEvent;
}
