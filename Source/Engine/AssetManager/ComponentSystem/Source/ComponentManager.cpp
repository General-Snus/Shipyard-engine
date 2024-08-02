#include "Engine/AssetManager/AssetManager.pch.h"
#include "../ComponentManager.h" 
 

ComponentManagerBase::ComponentManagerBase(GameObjectManager* manager, std::string typeName) : myManager(manager), Comparator(typeName)
{
} 

bool ComponentManagerBase::ValidComponentType(const Component* cmp)
{
	return cmp->GetTypeInfo().Name() == Comparator;
}

 