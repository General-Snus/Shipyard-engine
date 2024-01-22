#pragma once
#include <cassert>  
#include <Engine/AssetManager/AssetManagerUtills.hpp>
#include <iostream>
#include <unordered_map>
#include <vector>
#include "UUID.h"
//Original creation by Simon

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

	ComponentManagerBase() = default;
	virtual ~ComponentManagerBase() = default;
	virtual void Destroy() = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual void DeleteGameObject(const SY::UUID aGameObjectID) = 0;
	virtual void CollidedWith(const SY::UUID aFirstID,const SY::UUID aTargetID) = 0;
	virtual void OnSiblingChanged(const SY::UUID aGameObjectID,const std::type_info* SourceClass = nullptr) = 0;

	void SetUpdatePriority(const UpdatePriority aPriority) { myUpdatePriority = aPriority; }
	const UpdatePriority GetUpdatePriority() const { return myUpdatePriority; }

private:
	UpdatePriority myUpdatePriority = ComponentManagerBase::UpdatePriority::Normal;
};


template <class T>
class ComponentManager : public ComponentManagerBase
{
public:
	ComponentManager() = default;
	~ComponentManager() = default;

	void Destroy() override;

	T& AddComponent(const SY::UUID aGameObjectID);

	T& AddComponent(const SY::UUID aGameObjectID,const T& aComponent);

	template <typename... Args>
	T& AddComponent(const SY::UUID aGameObjectID,Args... someParameters);

	const bool HasComponent(const SY::UUID aGameObjectID) const;

	std::vector<T>& GetAllComponents();

	T& GetComponent(const SY::UUID aGameObjectID);

	T* TryGetComponent(const SY::UUID aGameObjectID);

	void Update() override;
	void Render() override;
	void DeleteGameObject(const SY::UUID aGameObjectID) override;
	void CollidedWith(const SY::UUID aFirstID,const SY::UUID aTargetID) override;
	void OnSiblingChanged(const SY::UUID aGameObjectID,const std::type_info* SourceClass = nullptr) override;
private:
	std::unordered_map<SY::UUID,unsigned int> myGameObjectIDtoVectorIndex;
	std::unordered_map<unsigned int,SY::UUID> myVectorIndexToGameObjectID;
	std::vector<T> myComponents;
};

template<class T>
inline void ComponentManager<T>::Destroy()
{
	myGameObjectIDtoVectorIndex.clear();
	myVectorIndexToGameObjectID.clear();
	myComponents.clear();
}

template<class T>
T& ComponentManager<T>::AddComponent(const SY::UUID aGameObjectID)
{
	myGameObjectIDtoVectorIndex[aGameObjectID] = static_cast<unsigned int>(myComponents.size());
	myVectorIndexToGameObjectID[static_cast<unsigned int>(myComponents.size())] = aGameObjectID;
	myComponents.push_back(T(aGameObjectID));
	static_cast<Component*>(&myComponents.back())->Init();
	return myComponents.back();
}

template<class T>
inline T& ComponentManager<T>::AddComponent(const SY::UUID aGameObjectID,const T& aComponent)
{
	myGameObjectIDtoVectorIndex[aGameObjectID] = static_cast<unsigned int>(myComponents.size());
	myVectorIndexToGameObjectID[static_cast<unsigned int>(myComponents.size())] = aGameObjectID;
	myComponents.push_back(aComponent);
	static_cast<Component*>(&myComponents.back())->SetOwnerID(aGameObjectID);
	static_cast<Component*>(&myComponents.back())->Init();
	return myComponents.back();
}

template<class T>
template<typename...  Args>
inline T& ComponentManager<T>::AddComponent(const SY::UUID aGameObjectID,Args... someParameters)
{
	myGameObjectIDtoVectorIndex[aGameObjectID] = static_cast<unsigned int>(myComponents.size());
	myVectorIndexToGameObjectID[static_cast<unsigned int>(myComponents.size())] = aGameObjectID;
	myComponents.push_back(T(aGameObjectID,someParameters...));
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
	if (myGameObjectIDtoVectorIndex.find(aGameObjectID) != myGameObjectIDtoVectorIndex.end())
	{
		return &myComponents[myGameObjectIDtoVectorIndex[aGameObjectID]];
	}

	//std::cout << "ComponentManager: Tried to get a component but the game object did not exist.ID : " << aGameObjectID << " \n";
	//assert(false && "ComponentManager: Tried to get a component but the game object did not exist. ID: " + aGameObjectID);
	//ERROR_PRINT("ComponentManager: Tried to get a component but the game object did not exist. ID: " + aGameObjectID);
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
	auto it = myGameObjectIDtoVectorIndex.find(aGameObjectID);		// Find the game object
	if (it == myGameObjectIDtoVectorIndex.end())
	{
		return;			// If it doesn't exist, don't do anything
	}
	unsigned int index = it->second;								// Get the component index of the game object
	unsigned int id = myVectorIndexToGameObjectID[static_cast<unsigned int>(myComponents.size() - 1)]; // Get the id of the game object with the last component 
	myComponents[index].Destroy(); // Call custom destructor 
	myComponents[index] = myComponents[myComponents.size() - 1]; // Swap the last component with the component to remove (cyclic remove)
	//std::swap(myComponents[index],myComponents[myComponents.size() - 1]); // Swap the last component with the component to remove (cyclic remove)


	std::cout << "Removed " << typeid(T).name() << " at id: " << std::to_string(id) + "\n";
	myComponents.pop_back();										// Remove the newly last component (the component to remove)

	myGameObjectIDtoVectorIndex[id] = index;						// Change the previously last game object to refer to the new component index
	myVectorIndexToGameObjectID[index] = id;						// Change the index to refer to the game object

	// TODO: vvv THESE
	myVectorIndexToGameObjectID.erase(myVectorIndexToGameObjectID.at(static_cast<unsigned int>(myComponents.size())));// Remove the removed component from the game object id list
	myGameObjectIDtoVectorIndex.erase(it->first);					// Remove the removed game object id from the component list
}

template<class T>
void ComponentManager<T>::CollidedWith(const SY::UUID aFirstID,const SY::UUID aTargetID)
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
inline void ComponentManager<T>::OnSiblingChanged(const SY::UUID aGameObjectID,const std::type_info* SourceClass)
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
