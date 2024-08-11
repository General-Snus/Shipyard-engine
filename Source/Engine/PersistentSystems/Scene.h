#pragma once 
#include <Engine/AssetManager/ComponentSystem/GameObjectManager.h>
#include <Tools/Utilities/LinearAlgebra/Sphere.hpp>
#include <memory>

class Scene : public std::enable_shared_from_this<Scene>
{
public: 
	Scene();
	Scene(const Scene& other) = default;
	GameObjectManager& GetGOM() { return m_GameObjectManager; }
	static GameObjectManager& ActiveManager(); // this wont fly as you most likely will get the main scene by accident




	bool IsLoaded();
	std::string Name();
	std::string Path(); //TODO Let this be overriden by base asset later
	
	
	//Move to managerment, keep scene simple
	//void Save();
	//void Load();
	//void Unload();
	//
	////Merges to the scene it was called on
	//void Merge(const Scene& other);








private:
	GameObjectManager m_GameObjectManager;
	Sphere<float> m_WorldBounds;
}; 