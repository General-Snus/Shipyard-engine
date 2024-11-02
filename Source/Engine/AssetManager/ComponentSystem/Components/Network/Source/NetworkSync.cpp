#include "AssetManager.pch.h"

#include "../NetworkSync.h"

bool NetworkSync::InspectorView()
{
    if (!Component::InspectorView())
    {
        return false;
    }
    Reflect<NetworkSync>();
    return true;
}
