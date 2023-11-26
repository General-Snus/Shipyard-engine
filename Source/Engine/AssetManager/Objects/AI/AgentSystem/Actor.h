//MOTHBALLED
#ifdef  MOTHBALLED

#pragma once  
#include "Controllers/Controller.h" 
#include <filesystem>

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

	Vector3f GetPosition() const { return latestInformation.position; }
	Controller* GetController() const { return controller; } 
	void SetController(Controller* aController);

private:
	SteeringOutput Update(const SteeringInput& input);
	void SetPosition(const Vector3f aPosition) { latestInformation.position = aPosition; }
	SteeringInput latestInformation;
	Controller* controller; 
};
 
#endif //  MOTHBALLED