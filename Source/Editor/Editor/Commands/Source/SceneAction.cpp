#include "../SceneAction.h"
#include "Commands\CommandBuffer.h"
#include "Engine/AssetManager/ComponentSystem/Components/Transform.h"
#include "Engine/AssetManager/ComponentSystem/GameObjectManager.h"
#include "Engine\AssetManager\ComponentSystem\GameObject.h"
#include <Engine/PersistentSystems/Scene.h>
#include <cassert>
#include <format>

GameobjectAdded::GameobjectAdded(const GameObject object) : m_object(object)
{
	assert(object.IsValid());
	m_components = m_object.CopyAllComponents();
	data = m_object.scene().GetGOM().GetData(m_object.GetID());
	description = std::format("GameObject {} added", data.Name);
}

void GameobjectAdded::commandUndo()
{
	m_object.scene().GetGOM().DeleteGameObject(m_object.GetID(), true);
}

void GameobjectAdded::commandRedo()
{
	m_object.scene().GetGOM().CreateGameObject(m_object.GetID(), data);

	m_object.RemoveComponent<Transform>();
	//TODO default added transform overrides the copy transform
	for (const auto& i : m_components)
	{
		m_object.AddBaseComponent(i);
	}
}

bool GameobjectAdded::merge(BaseCommand* ptr)
{
	ptr;
	return false;
}

GameobjectDeleted::GameobjectDeleted(const GameObject object) : m_object(object)
{
	assert(object.IsValid());
	m_components = m_object.CopyAllComponents();
	data = m_object.scene().GetGOM().GetData(m_object.GetID());
	description = std::format("GameObject {} added", data.Name);
}

void GameobjectDeleted::commandUndo()
{
	m_object.scene().GetGOM().CreateGameObject(m_object.GetID(), data);

	m_object.RemoveComponent<Transform>();
	//TODO default added transform overrides the copy transform
	for (const auto& i : m_components)
	{
		m_object.AddBaseComponent(i);
	}
}

void GameobjectDeleted::commandRedo()
{
	m_object.scene().GetGOM().DeleteGameObject(m_object.GetID(), true);
}

bool GameobjectDeleted::merge(BaseCommand* ptr)
{
	ptr;
	return false;
}
