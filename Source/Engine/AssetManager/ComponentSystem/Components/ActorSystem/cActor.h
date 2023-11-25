#pragma once 
#include "../../Component.h"
#include <Engine/AssetManager/Objects/AI/AgentSystem/Actor.h>
#include <Engine/AssetManager/Objects/AI/AgentSystem/Controllers/Controller.h> 
 
class cActor : public Component
{
public:
	cActor(const unsigned int anOwnerID);
	~cActor() = default;  
	void Init() override;
	void Update() override;
	void Render() override;  

	Controller* GetController() const { return controller; }
	void SetController(Controller* aController);
private:
	Controller* controller = nullptr;
};

