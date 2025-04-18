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
	float      hookCooldown = 1.5f;
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
namespace PillarGame {
	void SpawnGround(Vector3f base);
	GameObject SpawnPillar(Vector3f base);
	void SpawnHooks(int amount, float radius,GameObject parent);
	void SpawnPlayer(int id, float radius,GameObject parent);
}
