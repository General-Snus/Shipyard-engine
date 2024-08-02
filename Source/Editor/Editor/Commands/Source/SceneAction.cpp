#include "../SceneAction.h"
#include "Engine/AssetManager/ComponentSystem/Component.h"
#include "Engine/AssetManager/ComponentSystem/GameObjectManager.h"
#include <Engine/PersistentSystems/Scene.h> 


GameobjectAdded::GameobjectAdded(const GameObject object) : m_object(object)
{
	assert(object.IsValid()); 
	m_components = m_object.CopyAllComponents();
}

void GameobjectAdded::Undo()
{
	m_object.scene().GetGOM().DeleteGameObject(m_object.GetID());
}

void GameobjectAdded::Do()
{
	m_object.scene().GetGOM().CreateGameObject(m_object.GetID());

	for (auto& i : m_components)
	{ 
		m_object.AddComponent(*i );
	}
}

bool GameobjectAdded::Merge(std::shared_ptr<BaseCommand>& ptr)
{
	ptr;
	return false;
}

GameobjectDeleted::GameobjectDeleted(const GameObject object) : m_object(object)
{
	assert(object.IsValid());
}

void GameobjectDeleted::Undo()
{
	//m_object.scene().GetGOM().CreateGameObject();
}

void GameobjectDeleted::Do()
{
}

bool GameobjectDeleted::Merge(std::shared_ptr<BaseCommand>& ptr)
{
	ptr;
	return false;
}
