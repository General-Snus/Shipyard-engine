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
	const GameObject m_object;
};

class GameobjectDeleted : public BaseCommand
{
public:
	explicit GameobjectDeleted(const GameObject object);
	void Undo() override;
	void Do() override;

	bool Merge(std::shared_ptr<BaseCommand>& ptr) override;
private:
	const GameObject m_object;
};
