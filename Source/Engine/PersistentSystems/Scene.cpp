#include "Engine\PersistentSystems\PersistentSystems.pch.h"

#include "Scene.h"
#include <Editor/Editor/Core/Editor.h>

Scene::Scene(const std::string &sceneName) : m_GameObjectManager(*this), SceneName(sceneName)
{
}

GameObjectManager &Scene::ActiveManager()
{
    return EditorInstance.GetActiveScene()->GetGOM();
}

void Scene::Merge(const Scene &scene)
{
    m_WorldBounds.ExpandSphere(scene.m_WorldBounds);
    m_GameObjectManager.Merge(scene.m_GameObjectManager);
}
