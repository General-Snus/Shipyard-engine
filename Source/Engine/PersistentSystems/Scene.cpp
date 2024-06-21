#include "Engine\PersistentSystems\PersistentSystems.pch.h"

#include "Scene.h"
#include <Editor/Editor/Core/Editor.h>

Scene::Scene()
{
}

GameObjectManager& Scene::ActiveManager()
{
	return Editor::GetMainScene()->GetGOM();
}
