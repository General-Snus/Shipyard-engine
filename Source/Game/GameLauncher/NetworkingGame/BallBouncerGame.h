#pragma once
#include "Engine\AssetManager\ComponentSystem\Component.h"
#include "Engine\AssetManager\Reflection\Reflectable.h"

class BallTag : public Component, public Reflectable<BallTag>
{
public:
	reflectable(BallTag);
	defaultComponentInspector();
	BallTag(const SY::UUID anOwnerId, GameObjectManager* aManager);
};

REFL_AUTO(type(BallTag))

class BallGameController : public Component, public Reflectable<BallGameController>
{
public:
	reflectable(BallGameController);
	defaultComponentInspector();
	BallGameController(const SY::UUID anOwnerId, GameObjectManager* aManager);

	int maxBallsInGame = 10;
	float ballSpawnCooldown = 1.0f;
};

REFL_AUTO(type(BallGameController), field(maxBallsInGame), field(ballSpawnCooldown))
 

namespace BallEradicationGame {
	GameObject MakePlayer(Vector3f position);
	GameObject MakeBall(Vector3f position);
	GameObject MakeArena(Vector3f position, Vector3f Rect);
}
