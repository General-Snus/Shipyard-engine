#include "../ComponentManager.h"
#include "Engine/AssetManager/AssetManager.pch.h"

ComponentManagerBase::ComponentManagerBase(GameObjectManager *manager, std::string typeName)
    : myManager(manager), Comparator(typeName)
{
}

bool ComponentManagerBase::ValidComponentType(const Component *cmp)
{
    return cmp->GetTypeInfo().Name() == Comparator;
}
 