#include "AssetManager.pch.h"

#include "../GameObject.h"
#include "Engine/AssetManager/ComponentSystem/Components/Transform.h"
#include "Engine/PersistentSystems/Scene.h"

GameObject GameObject::Create(const std::string& name, std::shared_ptr<Scene> ref)
{
	const auto object = ref ? ref->GetGOM().CreateGameObject() : Scene::activeManager().CreateGameObject();
	object.SetName(name);
	return object;
}

GameObject::GameObject() = default;
GameObject::~GameObject() = default;

Transform& GameObject::transform() const
{
	return GetComponent<Transform>();
}

Scene& GameObject::scene() const
{
	return myManager->m_OwnerScene;
}

Component* GameObject::AddBaseComponent(const Component* aComponent) const
{
	assert(myManager != nullptr && "GameObject has no manager");
	assert(myID.IsValid() && "GameObject has no ID");
	return myManager->AddBaseComponent(myID, aComponent);
}

GameObject::operator bool() const
{
	return IsValid();
}

std::vector<Component*> GameObject::GetAllComponents() const
{
	assert(myManager != nullptr && "GameObject has no manager");
	assert(myID.IsValid() && "GameObject has no ID");
	return myManager->GetAllAttachedComponents(myID);
}

std::vector<Component*> GameObject::CopyAllComponents() const
{
	assert(myManager != nullptr && "GameObject has no manager");
	assert(myID.IsValid() && "GameObject has no ID");
	return myManager->CopyAllAttachedComponents(myID);
}

bool GameObject::GetActive() const
{
	assert(myManager != nullptr && "GameObject has no manager");
	assert(myID.IsValid() && "GameObject has no ID");
	return myManager->GetActive(myID);
}

bool GameObject::operator==(const GameObject& other) const
{
	return !(myID != other.myID || myManager != other.myManager);
}

GameObject::operator SY::UUID() const
{
	return myID;
}

GameObject::operator std::string() const
{
	return GetName();
}

void GameObject::SetActive(const bool aState) const
{
	assert(myManager != nullptr && "GameObject has no manager");
	assert(myID.IsValid() && "GameObject has no ID");
	myManager->SetActive(myID, aState);
}

void GameObject::OnSiblingChanged(const std::type_info* SourceClass) const
{
	assert(myManager != nullptr && "GameObject has no manager");
	assert(myID.IsValid() && "GameObject has no ID");
	myManager->OnSiblingChanged(myID, SourceClass);
}
