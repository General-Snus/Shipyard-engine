#pragma once
#include "CommandBuffer.h"
#include <memory>
#include <Engine/AssetManager/ComponentSystem/Gameobject.h>
#include <Tools/Logging/Logging.h>

class GameobjectAdded : public BaseCommand
{
public:
	explicit GameobjectAdded(const GameObject object);
	void Undo() override;
	void Do() override;

	bool Merge(std::shared_ptr<BaseCommand>& ptr) override;
private:
	  GameObject m_object;
	  GameObjectManager::GameObjectData data;
	std::vector<Component*> m_components;
};

class GameobjectDeleted : public BaseCommand
{
public:
	explicit GameobjectDeleted(const GameObject object);
	void Undo() override;
	void Do() override;

	bool Merge(std::shared_ptr<BaseCommand>& ptr) override;
private:
	GameObject m_object;
	GameObjectManager::GameObjectData data;
	std::vector<Component*> m_components;
}; 