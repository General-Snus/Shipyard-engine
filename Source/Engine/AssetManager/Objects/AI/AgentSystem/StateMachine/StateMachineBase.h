#pragma once
#include <memory>
#include <Tools/Utilities/TemplateHelpers.h>
#include <unordered_map>


class StateMachineBase;
class StateBase
{
public:
	StateBase() = default;
	virtual ~StateBase() = default;

	virtual void Init(StateMachineBase* base);
	virtual void Enter() = 0;
	virtual void Update(float deltaTime,GameObject input = GameObject()) = 0;
	virtual void Exit() = 0;
protected:
	StateMachineBase* myStateMachine;
};

class StateMachineBase
{
public:
	friend class StateBase;
	StateMachineBase() = default;
	~StateMachineBase() = default;
	void Update(GameObject input);

	// -1 is defined no state	
	void ChangeState(int aStateID);
	// -1 is defined no state	 
	void AddState(int aStateID,std::shared_ptr<StateBase> arg);
	// -1 is defined no state	
	void RemoveState(int aStateID);
	// -1 is defined no state	
	void SetState(int aStateID);
	// -1 is defined no state	
	std::shared_ptr<StateBase> GetState(int aStateID);

	std::shared_ptr<StateBase> GetCurrentState();
	// -1 is defined no state	
	int GetCurrentStateID();
private:
	int myCurrentState;
	std::unordered_map<unsigned,std::shared_ptr<StateBase>> mySavedStates;
};

