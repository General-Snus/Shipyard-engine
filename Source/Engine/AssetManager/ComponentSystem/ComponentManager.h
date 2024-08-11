
#ifndef ComponentManagerDef 
#define ComponentManagerDef  
#include <cassert>
#include <unordered_map>
#include <vector>
#include <string>
#include "UUID.h"
#include <Tools/Reflection/refl.hpp>

class GameObjectManager;
class Component;

class ComponentManagerBase
{
public:
	enum class UpdatePriority
	{
		Transform,
		Normal = 100,
		Physics,
		Collision,
		Render
	};

	ComponentManagerBase(GameObjectManager* manager, std::string typeName);
	virtual ~ComponentManagerBase() = default;
	virtual void Destroy() = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual void DeleteGameObject(const SY::UUID aGameObjectID) = 0;
	virtual void CollidedWith(const SY::UUID aFirstID, const SY::UUID aTargetID) = 0;
	virtual void OnSiblingChanged(const SY::UUID aGameObjectID, const std::type_info* SourceClass = nullptr) = 0;

	virtual Component* AddComponent(SY::UUID aGameObjectID, const Component* aComponent) = 0;
	virtual bool ValidComponentType(const Component* cmp);

	void SetUpdatePriority(const UpdatePriority aPriority) { myUpdatePriority = aPriority; }
	UpdatePriority GetUpdatePriority() const { return myUpdatePriority; }

	//Cost: 1 Contains,1 Get, 1 reintp cast
	virtual Component* TryGetBaseComponent(const SY::UUID aGameObjectID) = 0;

	virtual Component* DeepCopy(const Component* cmp) = 0;
	virtual Component* DeepCopy(const SY::UUID aGameObjectID) = 0;

protected:
	std::string Comparator;
	GameObjectManager* myManager = nullptr;

private:
	UpdatePriority myUpdatePriority = ComponentManagerBase::UpdatePriority::Normal;
};


template <class T>
class ComponentManager : public ComponentManagerBase
{
public:
	ComponentManager(GameObjectManager* manager) : ComponentManagerBase(manager, refl::reflect<T>().name.str()) { };
	~ComponentManager() = default;

	void Destroy() override;

	Component* AddComponent(SY::UUID aGameObjectID, const Component* aComponent) override;
	T& AddComponent(const SY::UUID aGameObjectID);
	T& AddComponent(const SY::UUID aGameObjectID, const T& aComponent);
	template <typename... Args>
	T& AddComponent(const SY::UUID aGameObjectID, Args... someParameters);

	Component* TryGetBaseComponent(const SY::UUID aGameObjectID) override;

	const bool HasComponent(const SY::UUID aGameObjectID) const;
	//Cost: 1 Get, 
	std::vector<T>& GetAllComponents();
	//Cost: 1 Get, 
	T& GetComponent(const SY::UUID aGameObjectID);
	//Cost: 1 Contains,1 Get, 
	T* TryGetComponent(const SY::UUID aGameObjectID);

	Component* DeepCopy(const Component* cmp) override;
	Component* DeepCopy(const SY::UUID aGameObjectID) override;

	void Update() override;
	void Render() override;
	void DeleteGameObject(const SY::UUID aGameObjectID) override;
	void CollidedWith(const SY::UUID aFirstID, const SY::UUID aTargetID) override;
	void OnSiblingChanged(const SY::UUID aGameObjectID, const std::type_info* SourceClass = nullptr) override;
private:
	std::unordered_map<SY::UUID, unsigned int> myGameObjectIDtoVectorIndex;
	std::unordered_map<unsigned int, SY::UUID> myVectorIndexToGameObjectID;
	std::vector<T> myComponents;
};


template<class T>
inline void ComponentManager<T>::Destroy()
{
	myGameObjectIDtoVectorIndex.clear();
	myVectorIndexToGameObjectID.clear();
	myComponents.clear();
}


template <class T>
Component* ComponentManager<T>::TryGetBaseComponent(const SY::UUID aGameObjectID)
{
	return reinterpret_cast<Component*>(TryGetComponent(aGameObjectID));
}

template<class T>
T& ComponentManager<T>::AddComponent(const SY::UUID aGameObjectID)
{
	if (HasComponent(aGameObjectID))
	{
		return myComponents[myGameObjectIDtoVectorIndex[aGameObjectID]];
	}

	myGameObjectIDtoVectorIndex[aGameObjectID] = static_cast<unsigned int>(myComponents.size());
	myVectorIndexToGameObjectID[static_cast<unsigned int>(myComponents.size())] = aGameObjectID;
	myComponents.push_back(T(aGameObjectID, myManager));
	static_cast<Component*>(&myComponents.back())->SetManager(myManager);
	static_cast<Component*>(&myComponents.back())->Init();
	return myComponents.back();
}

template<class T>
inline Component* ComponentManager<T>::AddComponent(const SY::UUID aGameObjectID, const Component* aComponent)
{
	const T* cmp = dynamic_cast<const T*>(aComponent);
	if (cmp != nullptr)
	{
		return &AddComponent(aGameObjectID, *cmp);
	}
	return nullptr;
}

template<class T>
inline T& ComponentManager<T>::AddComponent(const SY::UUID aGameObjectID, const T& aComponent)
{
	if (HasComponent(aGameObjectID))
	{
		return myComponents[myGameObjectIDtoVectorIndex[aGameObjectID]];
	}

	myGameObjectIDtoVectorIndex[aGameObjectID] = static_cast<unsigned int>(myComponents.size());
	myVectorIndexToGameObjectID[static_cast<unsigned int>(myComponents.size())] = aGameObjectID;
	myComponents.push_back(aComponent);
	static_cast<Component*>(&myComponents.back())->SetOwnerID(aGameObjectID);
	static_cast<Component*>(&myComponents.back())->SetManager(myManager);
	static_cast<Component*>(&myComponents.back())->Init();
	return myComponents.back();
}

template<class T>
template<typename...  Args>
inline T& ComponentManager<T>::AddComponent(const SY::UUID aGameObjectID, Args... someParameters)
{
	if (HasComponent(aGameObjectID))
	{
		return myComponents[myGameObjectIDtoVectorIndex[aGameObjectID]];
	}

	myGameObjectIDtoVectorIndex[aGameObjectID] = static_cast<unsigned int>(myComponents.size());
	myVectorIndexToGameObjectID[static_cast<unsigned int>(myComponents.size())] = aGameObjectID;
	myComponents.push_back(T(aGameObjectID, myManager, someParameters...));
	static_cast<Component*>(&myComponents.back())->SetManager(myManager);
	static_cast<Component*>(&myComponents.back())->Init();
	return myComponents.back();
}

template<class T>
inline const bool ComponentManager<T>::HasComponent(const SY::UUID aGameObjectID) const
{
	return myGameObjectIDtoVectorIndex.find(aGameObjectID) != myGameObjectIDtoVectorIndex.end();
}

template<class T>
inline std::vector<T>& ComponentManager<T>::GetAllComponents()
{
	return myComponents;
}

template<class T>
inline T& ComponentManager<T>::GetComponent(const SY::UUID aGameObjectID)
{
	assert(myGameObjectIDtoVectorIndex.find(aGameObjectID) != myGameObjectIDtoVectorIndex.end() && "ComponentManager::GetComponent(...) tried to get a missing component. Maybe it's best if you use TryGetComponent instead.");
	return myComponents[myGameObjectIDtoVectorIndex[aGameObjectID]];
}

template<class T>
T* ComponentManager<T>::TryGetComponent(const SY::UUID aGameObjectID)
{
	if (myGameObjectIDtoVectorIndex.contains(aGameObjectID))
	{
		return &myComponents[myGameObjectIDtoVectorIndex[aGameObjectID]];
	}
	return nullptr;
}

template <class T>
Component* ComponentManager<T>::DeepCopy(const Component* cmp)
{
	const T* cmpT = dynamic_cast<const T*>(cmp);
	if (cmpT != nullptr)
	{
		return new T(*cmpT);
	}
	return nullptr;
}

template <class T>
Component* ComponentManager<T>::DeepCopy(const SY::UUID aGameObjectID)
{
	if (myGameObjectIDtoVectorIndex.contains(aGameObjectID))
	{
		const T* cmpT = dynamic_cast<const T*>(&myComponents[myGameObjectIDtoVectorIndex[aGameObjectID]]);
		if (cmpT != nullptr)
		{
			return new T(*cmpT);
		}
	}
	return nullptr;
}


template<class T>
void ComponentManager<T>::Update()
{
	for (size_t i = 0; i < myComponents.size(); i++)
	{
		if (myComponents[i].IsActive() && !myComponents[i].IsAdopted())
		{
			myComponents[i].Update();
		}
	}
}

template<class T>
void ComponentManager<T>::Render()
{
	for (size_t i = 0; i < myComponents.size(); i++)
	{
		if (myComponents[i].IsActive())
		{
			myComponents[i].Render();
		}
	}
}

template<class T>
void ComponentManager<T>::DeleteGameObject(const SY::UUID aGameObjectID)
{
	const auto it = myGameObjectIDtoVectorIndex.find(aGameObjectID);
	if (it == myGameObjectIDtoVectorIndex.end())
	{
		return;
	}
	const unsigned int index = it->second;
	const unsigned int id = myVectorIndexToGameObjectID[static_cast<unsigned int>(myComponents.size() - 1)];
	myComponents[index].Destroy();
	myComponents[index] = myComponents[myComponents.size() - 1];
	//std::swap(myComponents[index],myComponents[myComponents.size() - 1]); 
	myComponents.pop_back();
	 
	myGameObjectIDtoVectorIndex[id] = index;
	myVectorIndexToGameObjectID[index] = id;

	myVectorIndexToGameObjectID.erase(myVectorIndexToGameObjectID.at(static_cast<unsigned int>(myComponents.size())));
	myGameObjectIDtoVectorIndex.erase(it->first);
}

template<class T>
void ComponentManager<T>::CollidedWith(const SY::UUID aFirstID, const SY::UUID aTargetID)
{
	if (myGameObjectIDtoVectorIndex.find(aFirstID) != myGameObjectIDtoVectorIndex.end())
	{
		myComponents[myGameObjectIDtoVectorIndex[aFirstID]].CollidedWith(aTargetID);
	}

	if (myGameObjectIDtoVectorIndex.find(aTargetID) != myGameObjectIDtoVectorIndex.end())
	{
		myComponents[myGameObjectIDtoVectorIndex[aTargetID]].CollidedWith(aFirstID);
	}

}

template<class T>
inline void ComponentManager<T>::OnSiblingChanged(const SY::UUID aGameObjectID, const std::type_info* SourceClass)
{
	if (SourceClass == &typeid(T))
	{
		return;
	}

	if (myGameObjectIDtoVectorIndex.find(aGameObjectID) != myGameObjectIDtoVectorIndex.end())
	{
		myComponents[myGameObjectIDtoVectorIndex[aGameObjectID]].OnSiblingChanged(SourceClass);
	}
}

#endif