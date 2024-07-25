#include "../SceneAction.h"
#include <Engine/PersistentSystems/Scene.h>

GameobjectAdded::GameobjectAdded(const GameObject object) : m_object(object)
{
	assert(object.IsValid());
}

void GameobjectAdded::Undo()
{
}

void GameobjectAdded::Do()
{
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
