#include "Engine/AssetManager/AssetManager.pch.h"
#include "../GameObject.h" 
#include "Engine/AssetManager/ComponentSystem/Components/Transform.h" 
#include "Engine/PersistentSystems/Scene.h"

GameObject GameObject::Create(std::shared_ptr<Scene> ref)
{
	return ref ? ref->GetGOM().CreateGameObject() : Scene::ActiveManager().CreateGameObject();
}
  Transform& GameObject::transform() const
{
	return GetComponent<Transform>();
}

  Scene& GameObject::scene() const
{
	return myManager->m_OwnerScene;
}

  Component* GameObject::AddComponent(const Component* aComponent)
{
	assert(myManager != nullptr && "GameObject has no manager");
	assert(myID.IsValid() && "GameObject has no ID");
	return &myManager->AddComponent<Component>(myID, *aComponent);
}
  std::vector<Component*> GameObject::GetAllComponents() const
{
	assert(myManager != nullptr && "GameObject has no manager");
	assert(myID.IsValid() && "GameObject has no ID");
	return myManager->GetAllAttachedComponents(myID);
}

