#pragma once
#include <Engine/AssetManager/ComponentSystem/Component.h>

class PlayerComponent : public Component
{
  public:
	using Component::Component;
	ReflectableTypeRegistration();

	int playerId = 0;
	GameObject currentHook;
	GameObject wantToHook;
	bool isDragging = false;
};
REFL_AUTO(type(PlayerComponent), field(playerId), field(currentHook), field(wantToHook), field(isDragging))

class HookComponent : public Component
{
  public:
	using Component::Component;
	ReflectableTypeRegistration();

	bool hasConnection;
	GameObject connection;
};
REFL_AUTO(type(HookComponent), field(hasConnection), field(connection))

void SpawnGround(Vector3f base);
void SpawnPillar(Vector3f base);
void SpawnHooks(int amount, float radius, Vector3f base);
void SpawnPlayer(int id, float radius, Vector3f base);