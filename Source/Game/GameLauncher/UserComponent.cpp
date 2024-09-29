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
}

bool UserComponent::InspectorView()
{
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
