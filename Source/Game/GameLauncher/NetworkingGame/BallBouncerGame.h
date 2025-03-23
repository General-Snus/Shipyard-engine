#pragma once
class BallTag: public Component
{
public:
	using Component::Component;
	ReflectableTypeRegistration();
	defaultInspector(); 
};

REFL_AUTO(type(BallTag))

class BallGameController: public Component
{
public:
	using Component::Component;
	ReflectableTypeRegistration();
	defaultInspector();
	
	int maxBallsInGame = 10;
	float ballSpawnCooldown = 1.0f;
};

REFL_AUTO(type(BallGameController),field(maxBallsInGame),field(ballSpawnCooldown))

namespace BallEradicationGame {
	GameObject MakeBall(Vector3f position); 
	GameObject MakeArena(Vector3f position,Vector3f Rect);
}