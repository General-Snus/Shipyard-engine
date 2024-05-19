#pragma once 
#include <Engine/AssetManager/Objects/AI/AgentSystem/Actor.h>
#include <Engine/AssetManager/Objects/AI/AgentSystem/Controllers/Controller.h> 
#include "../../Component.h"

class cActor : public Component
{
public:
	cActor(const SY::UUID anOwnerID);
	~cActor() override = default;
	void Init() override;
	void Update() override;
	void Render() override;

	Controller* GetController() const { return controller; }
	void SetController(Controller* aController);
private:
	Controller* controller = nullptr;
};


REFL_AUTO(type(cActor))