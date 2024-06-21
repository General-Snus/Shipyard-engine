#include "Engine/AssetManager/AssetManager.pch.h"
#include "../GameObject.h" 
 
#include "Engine/PersistentSystems/Scene.h"

GameObject GameObject::Create(std::shared_ptr<Scene> ref)
{ 
	return ref ? ref->GetGOM().CreateGameObject() : Scene::ActiveManager().CreateGameObject();
}
