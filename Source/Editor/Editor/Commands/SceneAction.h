#pragma once
#include "CommandBuffer.h"
#include "Engine\AssetManager\ComponentSystem\Component.h"
#include "Engine\AssetManager\ComponentSystem\GameObjectManager.h"
#include <Engine/AssetManager/ComponentSystem/Gameobject.h>
#include <vector>

class GameobjectAdded : public BaseCommand
{
public:
	explicit GameobjectAdded(GameObject object);
	void     commandUndo() override;
	void     commandRedo() override;

	bool merge(BaseCommand* ptr) override;

private:
	GameObject                        m_object;
	GameObjectManager::GameObjectData data;
	std::vector<Component*>           m_components;
};

class GameobjectDeleted : public BaseCommand
{
public:
	explicit GameobjectDeleted(GameObject object);
	void     commandUndo() override;
	void     commandRedo() override;

	bool merge(BaseCommand* ptr) override;

private:
	GameObject                        m_object;
	GameObjectManager::GameObjectData data;
	std::vector<Component*>           m_components;
};
