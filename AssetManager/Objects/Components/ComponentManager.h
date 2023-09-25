#pragma once
#include <unordered_map>
#include <vector>
#include <cassert> 
#include <TGAFbx.h> 

//Original creation by Simon

class Component; 
class ComponentManagerBase
{
public:
	enum class UpdatePriority
	{
		Normal = 100,
		Collision,
		Render
	};

	ComponentManagerBase() = default;
	virtual ~ComponentManagerBase() = default;
	virtual void Destroy() = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual void DeleteGameObject(const unsigned int aGameObjectID) = 0;
	virtual void CollidedWith(const unsigned int aFirstID,const unsigned int aTargetID) = 0;

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

	void AddComponent(const unsigned int aGameObjectID);

	void AddComponent(const unsigned int aGameObjectID,const T& aComponent);

	template <typename... Args>
	void AddComponent(const unsigned int aGameObjectID,Args... someParameters);

	const bool HasComponent(const unsigned int aGameObjectID) const;

	std::vector<T>& GetAllComponents();

	T& GetComponent(const unsigned int aGameObjectID);

	T* TryGetComponent(const unsigned int aGameObjectID);

	void Update() override;
	void Render() override;
	void DeleteGameObject(const unsigned int aGameObjectID) override;
	void CollidedWith(const unsigned int aFirstID,const unsigned int aTargetID) override;

private:
	std::unordered_map<unsigned int,unsigned int> myGameObjectIDtoVectorIndex;
	std::unordered_map<unsigned int,unsigned int> myVectorIndexToGameObjectID;
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
void ComponentManager<T>::AddComponent(const unsigned int aGameObjectID)
{
	myGameObjectIDtoVectorIndex[aGameObjectID] = static_cast<unsigned int>(myComponents.size());
	myVectorIndexToGameObjectID[static_cast<unsigned int>(myComponents.size())] = aGameObjectID;
	myComponents.push_back( T(aGameObjectID));
}

template<class T>
inline void ComponentManager<T>::AddComponent(const unsigned int aGameObjectID,const T& aComponent)
{
	myGameObjectIDtoVectorIndex[aGameObjectID] = static_cast<unsigned int>(myComponents.size());
	myVectorIndexToGameObjectID[static_cast<unsigned int>(myComponents.size())] = aGameObjectID;
	myComponents.push_back(aComponent);
	static_cast<Component*>(&myComponents.back())->SetOwnerID(aGameObjectID);
}

template<class T>
template<typename...  Args>
inline void ComponentManager<T>::AddComponent(const unsigned int aGameObjectID,Args... someParameters)
{
	myGameObjectIDtoVectorIndex[aGameObjectID] = static_cast<unsigned int>(myComponents.size());
	myVectorIndexToGameObjectID[static_cast<unsigned int>(myComponents.size())] = aGameObjectID;
	myComponents.push_back(T(aGameObjectID,someParameters...));
}

template<class T>
inline const bool ComponentManager<T>::HasComponent(const unsigned int aGameObjectID) const
{
	return myGameObjectIDtoVectorIndex.find(aGameObjectID) != myGameObjectIDtoVectorIndex.end();
}

template<class T>
inline std::vector<T>& ComponentManager<T>::GetAllComponents()
{
	return myComponents;
}

template<class T>
inline T& ComponentManager<T>::GetComponent(const unsigned int aGameObjectID)
{
	assert(myGameObjectIDtoVectorIndex.find(aGameObjectID) != myGameObjectIDtoVectorIndex.end() && "ComponentManager::GetComponent(...) tried to get a missing component. Maybe it's best if you use TryGetComponent instead.");
	return myComponents[myGameObjectIDtoVectorIndex[aGameObjectID]];
}

template<class T>
T* ComponentManager<T>::TryGetComponent(const unsigned int aGameObjectID)
{
	if(myGameObjectIDtoVectorIndex.find(aGameObjectID) != myGameObjectIDtoVectorIndex.end())
	{
		return &myComponents[myGameObjectIDtoVectorIndex[aGameObjectID]];
	}
	
	std::cout << "ComponentManager: Tried to get a component but the game object did not exist.ID : " << aGameObjectID << " \n";
	//assert(false && "ComponentManager: Tried to get a component but the game object did not exist. ID: " + aGameObjectID);
	//ERROR_PRINT("ComponentManager: Tried to get a component but the game object did not exist. ID: " + aGameObjectID);
	return nullptr;
}

template<class T>
void ComponentManager<T>::Update()
{
	for(size_t i = 0; i < myComponents.size(); i++)
	{
		if(myComponents[i].IsActive())
		{
			myComponents[i].Update();
		}
	}
}

template<class T>
void ComponentManager<T>::Render()
{
	for(size_t i = 0; i < myComponents.size(); i++)
	{
		if(myComponents[i].IsActive())
		{
			myComponents[i].Render();
		}
	}
}

template<class T>
void ComponentManager<T>::DeleteGameObject(const unsigned int aGameObjectID)
{
	auto it = myGameObjectIDtoVectorIndex.find(aGameObjectID);		// Find the game object
	if(it == myGameObjectIDtoVectorIndex.end()) return;			// If it doesn't exist, don't do anything

	unsigned int index = it->second;								// Get the component index of the game object
	unsigned int id = myVectorIndexToGameObjectID[static_cast<unsigned int>(myComponents.size() - 1)]; // Get the id of the game object with the last component

	std::swap(myComponents[index],myComponents[myComponents.size() - 1]); // Swap the last component with the component to remove (cyclic remove)
	myComponents.pop_back();										// Remove the newly last component (the component to remove)

	myGameObjectIDtoVectorIndex[id] = index;						// Change the previously last game object to refer to the new component index
	myVectorIndexToGameObjectID[index] = id;						// Change the index to refer to the game object

	// TODO: vvv THESE
	myVectorIndexToGameObjectID.erase(myVectorIndexToGameObjectID.find(static_cast<unsigned int>(myComponents.size())));// Remove the removed component from the game object id list
	myGameObjectIDtoVectorIndex.erase(it->first);					// Remove the removed game object id from the component list
}

template<class T>
void ComponentManager<T>::CollidedWith(const unsigned int aFirstID,const unsigned int aTargetID)
{
	if(myGameObjectIDtoVectorIndex.find(aFirstID) != myGameObjectIDtoVectorIndex.end())
	{
		myComponents[myGameObjectIDtoVectorIndex[aFirstID]].CollidedWith(aTargetID);
	}

	if(myGameObjectIDtoVectorIndex.find(aTargetID) != myGameObjectIDtoVectorIndex.end())
	{
		myComponents[myGameObjectIDtoVectorIndex[aTargetID]].CollidedWith(aFirstID);
	}

}
