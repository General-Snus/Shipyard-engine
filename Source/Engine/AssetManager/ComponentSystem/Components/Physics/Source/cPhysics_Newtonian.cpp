#include "../cPhysics_Newtonian.h"
#include "AssetManager.pch.h"

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

void cPhysics_Newtonian::Render()
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
