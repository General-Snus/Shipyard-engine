#pragma once
#include <memory>
#include <Engine/AssetManager/ComponentSystem/Gameobject.h>
#include <Tools/Logging/Logging.h>
#include "CommandBuffer.h"

class GameobjectAdded : public BaseCommand
{
public:
	explicit GameobjectAdded(GameObject object);
	void     commandUndo() override;
	void     commandRedo() override;

	bool merge(std::shared_ptr<BaseCommand>& ptr) override;

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

	bool merge(std::shared_ptr<BaseCommand>& ptr) override;

private:
	GameObject                        m_object;
	GameObjectManager::GameObjectData data;
	std::vector<Component*>           m_components;
};
