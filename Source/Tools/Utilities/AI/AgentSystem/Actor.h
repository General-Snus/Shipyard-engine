#pragma once  
#include "Controller.h" 
#include <filesystem>

struct SteeringOutput
{
	Vector3f movement;
	Vector3f rotation;
};

//An actor has a want but no way to achieve it
//An actor will be listend to by a c[omponent]Actor that can act upon the actors wants
class Actor 
{ 
	friend class AIEventManager;
	friend class cActor;
public:
	Actor() = default;
	Actor(const Actor&) = default;
	~Actor() = default;

	Vector3f GetPosition() const { return position; } 
	Controller* GetController() const { return controller; } 
	void SetController(Controller* aController);

private:
	Vector3f Update(float aTimeDelta);
	void SetPosition(const Vector3f aPosition) { position = aPosition; }
	Vector3f position; 
	Controller* controller; 
};
 