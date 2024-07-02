#pragma once 
#include <Engine/AssetManager/ComponentSystem/GameObjectManager.h>
#include <Tools/Utilities/LinearAlgebra/Sphere.hpp>
 

class Scene
{
public: 
	Scene() = default;
	Scene(const Scene& other) = default;
	GameObjectManager& GetGOM() { return myGameObjectManager; }
	static GameObjectManager& ActiveManager(); // this wont fly as you most likely will get the main scene by accident

private:
	GameObjectManager myGameObjectManager;
	Sphere<float> myWorldBounds;
}; 