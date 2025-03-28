#pragma once 
#include <Engine/AssetManager/Objects/AI/AgentSystem/Controllers/Controller.h>
#include <Engine/AssetManager/Objects/AI/AgentSystem/StateMachine/StateMachineBase.h>

class StateMachineController : public Controller
{
public:
	MYLIB_REFLECTABLE()
	explicit StateMachineController(StateMachineBase decicionTree);
	explicit StateMachineController();
	bool Update(GameObject input) override;
	bool ComponentRequirement(GameObject input) override;
private:
	StateMachineBase myMachine;
private:
	//Decicion functions 
};




REFL_AUTO(type(StateMachineController))
