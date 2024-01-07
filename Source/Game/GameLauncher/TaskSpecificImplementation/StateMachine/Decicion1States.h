#pragma once
#include <Engine/AssetManager/ComponentSystem/GameObject.h>
#include <Engine/AssetManager/Objects/AI/AgentSystem/StateMachine/StateMachineBase.h>

enum class eStates
{
	Dead = 0,
	Movement,
	Fighting,
	Fleeing,
	Healing
};



class DeadState : public StateBase
{
public:
	void Enter() override;
	void Update(float deltaTime,GameObject input) override;
	void Exit() override;
};

class MovementState : public StateBase
{
public:
	void Enter() override;
	void Update(float deltaTime,GameObject input) override;
	void Exit() override;
};



class FightingState : public StateBase
{
public:
	void Enter() override;
	void Update(float deltaTime,GameObject input) override;
	void Exit() override;
};

class FleeingState : public StateBase
{
public:
	void Enter() override;
	void Update(float deltaTime,GameObject input) override;
	void Exit() override;
};

class HealingState : public StateBase
{
public:
	void Enter() override;
	void Update(float deltaTime,GameObject input) override;
	void Exit() override;
};