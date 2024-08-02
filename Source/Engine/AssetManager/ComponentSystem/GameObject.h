
#ifndef GameObjectDef 
#define GameObjectDef 
#include "Engine/AssetManager/ComponentSystem/GameObjectManager.h"

class Transform;
class Scene;
class Component;

class GameObject
{
public:
	//If no default argument is provided, the function will create a new GameObject in the active scene
	static GameObject Create(std::shared_ptr<Scene> ref = nullptr);  // why not set scene to active scene by default? INCLUDE HELL
	GameObject() = default;
	~GameObject() = default;



	template <class T>
	T& AddComponent();

	template <class T>
	T& AddComponent(const T& aComponent);

	template <class T, typename... Args>
	T& AddComponent(Args... someParameters);

	Component* AddComponent(const Component* aComponent);

	template <class T>
	bool HasComponent() const;

	template <class T>
	T* TryGetComponent() const;

	template <class T>
	T& GetComponent() const;


	operator SY::UUID() const;
	operator std::string() const;
	bool operator==(const GameObject& other)  const;


	std::vector<Component*> GetAllComponents() const;
	std::vector<Component*> CopyAllComponents() const;

	bool GetActive() const;
	void SetActive(bool aState) const;

	std::string GetName() const { return myManager->GetName(myID); };;
	void SetName(const std::string& name) const { myManager->SetName(name, myID); };

	void OnSiblingChanged(const std::type_info* SourceClass = nullptr) const;

	void SetLayer(const Layer aLayer) const { myManager->SetLayer(myID, aLayer); };
	Layer GetLayer() const { return myManager->GetLayer(myID); };

	SY::UUID GetID() const { return myID; }
	bool IsValid() const { return myID.IsValid() && myManager != nullptr; }

	Transform& transform() const;
	Scene& scene() const;
private:
	friend class GameObjectManager; //Only the asset manager can create and destroy components 
	GameObject(const SY::UUID anID, GameObjectManager* aManager) : myID(anID), myManager(aManager) {}

	//DO NOT ADD VARIABLES HERE IT IS INCONSISTENT WITH PHILOSOPHY AND WILL NOT WORK
	SY::UUID myID = UINT_MAX; // this could be upped to 8 bytes without loss because of alignment
	GameObjectManager* myManager = nullptr;
};

__forceinline std::vector<Component*> GameObject::CopyAllComponents() const
{
	assert(myManager != nullptr && "GameObject has no manager");
	assert(myID.IsValid() && "GameObject has no ID");
	return myManager->CopyAllAttachedComponents(myID);
}
inline bool GameObject::GetActive() const
{
	assert(myManager != nullptr && "GameObject has no manager");
	assert(myID.IsValid() && "GameObject has no ID");
	return myManager->GetActive(myID);
}
template<class T>
T& GameObject::AddComponent()
{
	assert(myManager != nullptr && "GameObject has no manager");
	assert(myID.IsValid() && "GameObject has no ID");
	return myManager->AddComponent<T>(myID);
}

template<class T>
T& GameObject::AddComponent(const T& aComponent)
{
	assert(myManager != nullptr && "GameObject has no manager");
	assert(myID.IsValid() && "GameObject has no ID");
	return myManager->AddComponent<T>(myID, aComponent);
}


template<class T, typename ...Args>
T& GameObject::AddComponent(Args ...someParameters)
{
	assert(myManager != nullptr && "GameObject has no manager");
	assert(myID.IsValid() && "GameObject has no ID");
	return myManager->AddComponent<T>(myID, someParameters...);
}

template<class T>
bool GameObject::HasComponent() const
{
	assert(myManager != nullptr && "GameObject has no manager");
	assert(myID.IsValid() && "GameObject has no ID");
	return myManager->HasComponent<T>(myID);
}

template<class T>
T* GameObject::TryGetComponent() const
{
	assert(myManager != nullptr && "GameObject has no manager");
	assert(myID.IsValid() && "GameObject has no ID");
	return myManager ? myManager->TryGetComponent<T>(myID) : nullptr;
}

template<class T>
T& GameObject::GetComponent() const
{
	assert(myManager != nullptr && "GameObject has no manager");
	assert(myID.IsValid() && "GameObject has no ID");
	return myManager->GetComponent<T>(myID);
}

inline bool GameObject::operator==(const GameObject& other) const
{
	return !(myID != other.myID || myManager != other.myManager);
}
 

inline GameObject::operator SY::UUID() const
{
	return myID;
}
inline GameObject::operator std::string() const
{
	return GetName();
} 


inline void GameObject::SetActive(const bool aState) const
{
	assert(myManager != nullptr && "GameObject has no manager");
	assert(myID.IsValid() && "GameObject has no ID");
	myManager->SetActive(myID, aState);
}

inline void GameObject::OnSiblingChanged(const std::type_info* SourceClass) const
{
	assert(myManager != nullptr && "GameObject has no manager");
	assert(myID.IsValid() && "GameObject has no ID");
	myManager->OnSiblingChanged(myID, SourceClass);
}
#endif
