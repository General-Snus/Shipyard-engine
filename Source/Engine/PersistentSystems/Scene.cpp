#include "PersistentSystems.pch.h"

#include "Scene.h"
#include <Editor/Editor/Core/Editor.h>

Scene::Scene(const std::string& sceneName) : m_GameObjectManager(*this), SceneName(sceneName)
{
}

GameObjectManager& Scene::activeManager()
{
	return EDITOR_INSTANCE.GetActiveScene()->GetGOM();
}

void Scene::merge(const Scene& scene)
{
	m_WorldBounds.ExpandSphere(scene.m_WorldBounds);
	m_GameObjectManager.Merge(scene.m_GameObjectManager);
}
