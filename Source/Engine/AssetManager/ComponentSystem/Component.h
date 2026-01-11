#pragma once
#include <Engine\AssetManager\ComponentSystem\UUID.h>
#include <Engine\AssetManager\Interfaces.h> 
#include <Engine\AssetManager\Reflection\Reflectable.h>

class GameObjectManager;
class GameObject;
class Transform;

class Component : public Reflectable<Component>, public AvailableInInspector {
public:
	reflectable(Component)
		Component(const SY::UUID anOwnerID, GameObjectManager* aManager);
	Component(const Component& aComponent);
	virtual ~Component() noexcept = default;

	virtual void Init() {}
	virtual void Update() {}
	virtual void Destroy() {}
	virtual void OnSiblingChanged(const std::type_info* SourceClass = nullptr)
	{
		SourceClass;
	};

	template <class T> bool HasComponent() const;

	const SY::UUID GetOwner() const
	{
		return myOwnerID;
	}

	GameObject gameObject() const;

	template <class T> T& GetComponent();
	template <class T> T* TryGetComponent();
	template <class T> T* TryGetAddComponent();

	template <class T> const T& GetComponent() const;
	template <class T> const T* TryGetComponent() const;
	template <class T> const T* TryGetAddComponent() const;

	bool InspectorView() override;

	bool IsActive() const;
	void SetActive(const bool aState)
	{
		m_IsActive = aState;
	}

	virtual void OnColliderEnter(const SY::UUID /*aGameObjectID*/) {}

	virtual void OnColliderExit(const SY::UUID /*aGameObjectID*/) {}

	bool IsAdopted() const
	{
		return IsInherited;
	};
	void Adopt()
	{
		IsInherited++;
	}
	void Abandon();
	Transform& transform();
	const Transform& transform() const;

	SY::UUID myOwnerID;
	GameObjectManager* myManager = nullptr;

	// IsInherited is a new system that allows a component to remove an other component from the update loop and promise
	// to take care of it themselves
	int IsInherited = 0;
	bool m_IsActive = true;

	virtual void Rebase(const SY::UUID newBase, GameObjectManager* aManager)
	{
		SetOwnerID(myOwnerID + newBase);
		SetManager(aManager);
	}
protected:
	virtual bool ComponentInspectorHeader();
private:
	Component() = delete;

	template <class T> friend class ComponentManager;
	void SetOwnerID(const SY::UUID anOwnerID)
	{
		myOwnerID = anOwnerID;
	}

	void SetManager(GameObjectManager* aManager)
	{
		myManager = aManager;
	}

};

REFL_AUTO(type(Component), field(myOwnerID), field(m_IsActive))


#pragma region ComponentTemplates

template <class T> bool Component::HasComponent() const
{
	return myManager ? myManager->HasComponent<T>(myOwnerID) : false;
}

template <class T> T& Component::GetComponent()
{
	return myManager->GetComponent<T>(myOwnerID);
}

template <class T> T* Component::TryGetComponent()
{
	return myManager ? myManager->TryGetComponent<T>(myOwnerID) : nullptr;
}

template <class T> T* Component::TryGetAddComponent()
{
	if (auto* returnComponent = myManager->TryGetComponent<T>(myOwnerID))
	{
		return returnComponent;
	}
	return &myManager->AddComponent<T>(myOwnerID);
}

template <class T> const T& Component::GetComponent() const
{
	return myManager->GetComponent<T>(myOwnerID);
}

template <class T> const T* Component::TryGetComponent() const
{
	return myManager->TryGetComponent<T>(myOwnerID);
}

template <class T> const T* Component::TryGetAddComponent() const
{
	if (auto* returnComponent = myManager->TryGetComponent<T>(myOwnerID))
	{
		return returnComponent;
	}
	return myManager->AddComponent<T>(myOwnerID);
}
#pragma endregion 
