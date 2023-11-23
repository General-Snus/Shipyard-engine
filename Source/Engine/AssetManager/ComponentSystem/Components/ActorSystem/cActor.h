#pragma once 
#include "../../Component.h"
#include <Tools/Utilities/AI/AgentSystem/Actor.h>
#include <Tools/Utilities/AI/AgentSystem/Controller.h>
 
class cActor : public Component
{
public:
	cActor(const unsigned int anOwnerID);
	~cActor() = default;  
	void Init() override;
	void Update() override;
	void Render() override; 
	Controller* GetController() const;
	void SetController(Controller* aController);
	Actor* GetActor(); 
private:
	Actor* myActor = nullptr;
};

