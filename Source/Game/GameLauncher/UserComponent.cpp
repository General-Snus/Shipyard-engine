#include "ShipyardEngine.pch.h"

#include "UserComponent.h"

void UserComponent::Init()
{
}

void UserComponent::Start()
{
}

void UserComponent::Update()
{
	transform().Move(0, 0, -.25f * TimerInstance.getDeltaTime());

	if (Input.IsKeyHeld(Keys::UP))
	{
		transform().Move(0, 0, 1.f * TimerInstance.getDeltaTime());
	}
}

bool UserComponent::InspectorView()
{
	OPTICK_EVENT();
	if (!Component::InspectorView())
	{
		return false;
	}

	Reflect<UserComponent>();

	return false;
}

void UserComponent::OnColliderEnter(const SY::UUID aGameObjectID)
{
	UNREFERENCED_PARAMETER(aGameObjectID);
}

void UserComponent::OnColliderExit(const SY::UUID aGameObjectID)
{
	UNREFERENCED_PARAMETER(aGameObjectID);
}
