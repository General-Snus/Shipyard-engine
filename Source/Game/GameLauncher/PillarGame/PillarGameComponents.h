#pragma once
#include <Engine/AssetManager/ComponentSystem/Component.h>

class PlayerComponent : public Component
{
public:
	using Component::Component;
	ReflectableTypeRegistration();
	defaultInspector();

	int        playerId = 0;
	GameObject currentHook;
	GameObject wantToHook;
	bool       isDragging = false;
	float      timeSinceHook = 0.0f;
	float      hookCooldown = 10.2f;
};

REFL_AUTO(type(PlayerComponent), field(playerId), field(currentHook), field(wantToHook), field(isDragging),
          field(timeSinceHook), field(hookCooldown))

class HookComponent : public Component
{
public:
	using Component::Component;
	ReflectableTypeRegistration();
	defaultInspector();

	bool       hasConnection = false;
	GameObject connection;
};

REFL_AUTO(type(HookComponent), field(hasConnection), field(connection))

void SpawnGround(Vector3f base);
void SpawnPillar(Vector3f base);
void SpawnHooks(int amount, float radius, Vector3f base);
void SpawnPlayer(int id, float radius, Vector3f base);
