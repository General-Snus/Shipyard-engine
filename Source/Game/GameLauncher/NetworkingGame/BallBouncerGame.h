#pragma once
class BallTag: public Component
{
public:
	using Component::Component;
	ReflectableTypeRegistration();
	defaultInspector(); 
};

REFL_AUTO(type(BallTag))

namespace BallEradicationGame {
	GameObject MakeBall(Vector3f position);
	void MakeArena(Vector3f position,Vector3f Rect);
}