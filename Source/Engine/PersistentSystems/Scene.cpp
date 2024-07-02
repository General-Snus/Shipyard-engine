#include "Engine\PersistentSystems\PersistentSystems.pch.h"

#include "Scene.h"
#include <Editor/Editor/Core/Editor.h>
 
GameObjectManager& Scene::ActiveManager()
{
	return Editor::GetMainScene()->GetGOM();
}
