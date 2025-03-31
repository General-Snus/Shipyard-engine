#include "AssetManager.pch.h"

#include "../cPhysics_Newtonian.h"

cPhysics_Newtonian::cPhysics_Newtonian(const SY::UUID anOwnerId, GameObjectManager *aManager)
    : Component(anOwnerId, aManager)
{
}

void cPhysics_Newtonian::Init()
{
}

void cPhysics_Newtonian::Update()
{
}

bool cPhysics_Newtonian::InspectorView()
{
    if (!Component::InspectorView())
    {
        return false;
    }
    Reflect<cPhysics_Newtonian>();
    return true;
}
