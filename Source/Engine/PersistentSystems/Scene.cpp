#include "Engine\PersistentSystems\PersistentSystems.pch.h"

#include "Scene.h"
#include <Editor/Editor/Core/Editor.h>

Scene::Scene() : m_GameObjectManager(*this)
{
}

GameObjectManager& Scene::ActiveManager()
{
	return Editor::GetMainScene()->GetGOM();
}
