#include "AssetManager.pch.h"
#include "../StateMachineBase.h"
#include <Game/GameLauncher/TaskSpecificImplementation/StateMachine/Decicion1States.h>

void StateMachineBase::Update(GameObject input)
{
	if (myCurrentState != -1)
	{
		mySavedStates.at(myCurrentState)->Update(TimerInstance.getDeltaTime(), input);
	}
}

void StateMachineBase::ChangeState(int aStateID)
{
	if (myCurrentState != -1)
	{
		mySavedStates.at(myCurrentState)->Exit();
	}
	assert(mySavedStates.contains(aStateID)&& "State does not exist");
	myCurrentState = aStateID;
	mySavedStates.at(myCurrentState)->Enter();
	LOGGER.Log(std::format("Entered {} state", magic_enum::enum_name(static_cast<eStates>(myCurrentState))));
}

void StateMachineBase::AddState(int aStateID, std::shared_ptr<StateBase> arg)
{
	mySavedStates.try_emplace(aStateID, arg);
	arg->Init(this);
}

void StateMachineBase::RemoveState(int aStateID)
{
	assert(mySavedStates.contains(aStateID)&& "State does not exist");
	if (aStateID == myCurrentState)
	{
		myCurrentState = -1;
		mySavedStates.at(aStateID)->Exit();
	}
	mySavedStates.erase(aStateID);
}

void StateMachineBase::SetState(int aStateID)
{
	assert(mySavedStates.contains(aStateID)&& "State does not exist");
	if (myCurrentState != -1)
	{
		mySavedStates.at(myCurrentState)->Exit();
	}
	myCurrentState = aStateID;
	mySavedStates.at(myCurrentState)->Enter();
}

std::shared_ptr<StateBase> StateMachineBase::GetState(int aStateID)
{
	return mySavedStates.at(aStateID);
}

std::shared_ptr<StateBase> StateMachineBase::GetCurrentState()
{
	return mySavedStates.at(myCurrentState);
}

int StateMachineBase::GetCurrentStateID()
{
	return myCurrentState;
}

void StateBase::Init(StateMachineBase* aBase)
{
	myStateMachine = aBase;
}
