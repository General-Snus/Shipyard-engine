#pragma once
#include <memory>
#include <Engine/AssetManager/ComponentSystem/GameObjectManager.h>
#include <Tools/Utilities/LinearAlgebra/Sphere.hpp>

class Scene : public std::enable_shared_from_this<Scene>
{
public:
	Scene(const std::string& SceneName);

	GameObjectManager& GetGOM()
	{
		return m_GameObjectManager;
	}

	static GameObjectManager& activeManager(); // this wont fly as you most likely will get the main scene by accident

	// this <- other
	void merge(const Scene& other);
	void unload();

	bool        IsLoaded();
	std::string Name();
	std::string Path(); // TODO Let this be overriden by base asset later

	// Move to managerment, keep scene simple
	// void Save();
	// void Load();
	// void Unload();

private:
	GameObjectManager m_GameObjectManager;
	Sphere<float>     m_WorldBounds;
	const std::string SceneName;
};
