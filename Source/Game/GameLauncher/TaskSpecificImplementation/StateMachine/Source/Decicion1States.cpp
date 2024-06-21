#include "../Decicion1States.h"
#include <Engine/PersistentSystems/ArtificialInteligence/AICommands/AICommands.h>

#include <Engine/AssetManager/AssetManager.pch.h> 

#include "Engine/AssetManager/Objects/AI/AgentSystem/AIPollingManager.h"
#include "Engine/AssetManager/Objects/AI/AgentSystem/PollingStations/Target_PollingStation.h"

void MovementState::Enter()
{
	std::cout << "Entering MovementState" << std::endl;
}

void MovementState::Update(float deltaTime,GameObject input)
{
	deltaTime;
	if(GeneralizedAICommands::IsDead(input))
	{
		myStateMachine->ChangeState((int)eStates::Dead);
		return;
	}

	if(GeneralizedAICommands::IsHealthy(input))
	{
		if(GeneralizedAICommands::IsTargetAlive(input))
		{
			if(GeneralizedAICommands::IsTargetInRange(input))
			{
				myStateMachine->ChangeState((int)eStates::Fighting);
				return;
			}
		}
		else
		{
			if(!GeneralizedAICommands::IsFullyHealed(input))
			{
				myStateMachine->ChangeState((int)eStates::Fleeing);
				return;
			}
		}
	}
	else
	{
		myStateMachine->ChangeState((int)eStates::Fleeing);
		return;
	}
	GeneralizedAICommands::MoveFreely(input);
}

void MovementState::Exit()
{
	std::cout << "Exiting MovementState" << std::endl;
}

void DeadState::Enter()
{
	std::cout << "Entering DeadState" << std::endl;
}

void DeadState::Update(float deltaTime,GameObject input)
{
	deltaTime;
	if(GeneralizedAICommands::IsDead(input))
	{
		GeneralizedAICommands::DeathSpin(input);
	}
	else
	{
		myStateMachine->ChangeState((int)eStates::Movement);
	}
}

void DeadState::Exit()
{
	std::cout << "Exiting DeadState" << std::endl;
}

void FightingState::Enter()
{
	std::cout << "Entering FightingState" << std::endl;
}

void FightingState::Update(float deltaTime,GameObject input)
{
	deltaTime;
	if(GeneralizedAICommands::IsDead(input))
	{
		myStateMachine->ChangeState((int)eStates::Dead);
		return;
	}

	if(GeneralizedAICommands::IsHealthy(input))
	{
		if(GeneralizedAICommands::IsTargetInSight(input))
		{
			GeneralizedAICommands::ShootAtTarget(input);
		}
		else
		{
			GeneralizedAICommands::AlignToTarget(input);
		}

		if(!GeneralizedAICommands::IsTargetAlive(input) || !GeneralizedAICommands::IsTargetInRange(input))
		{
			myStateMachine->ChangeState((int)eStates::Movement);
		}
	}
	else
	{
		myStateMachine->ChangeState((int)eStates::Fleeing);
	}
}
void FightingState::Exit()
{
	std::cout << "Exiting FightingState" << std::endl;
}

void FleeingState::Enter()
{
	std::cout << "Entering FleeingState" << std::endl;
}

void FleeingState::Update(float deltaTime,GameObject input)
{
	deltaTime;
	if(GeneralizedAICommands::IsDead(input))
	{
		myStateMachine->ChangeState((int)eStates::Dead);
		return;
	}

	GeneralizedAICommands::Retreat(input);

	const Vector3f position = input.GetComponent<Transform>().GetPosition();
	Vector3f closestWell = AIPollingManager::Get().GetStation<MultipleTargets_PollingStation>("Healing")->GetClosestTargetPosition(position);


	if((closestWell - position).Length() < 1.0f)
	{
		myStateMachine->ChangeState((int)eStates::Healing);
	}
}

void FleeingState::Exit()
{
	std::cout << "Exiting FleeingState" << std::endl;
}

void HealingState::Enter()
{
	std::cout << "Entering HealingState" << std::endl;
}

void HealingState::Update(float deltaTime,GameObject input)
{
	deltaTime;
	if(GeneralizedAICommands::IsDead(input))
	{
		myStateMachine->ChangeState((int)eStates::Dead);
		return;
	}

	GeneralizedAICommands::Retreat(input);

	if(GeneralizedAICommands::IsHealthy(input))
	{
		if(GeneralizedAICommands::IsTargetAlive(input))
		{
			myStateMachine->ChangeState((int)eStates::Movement);
		}
	}
}

void HealingState::Exit()
{
	std::cout << "Exiting HealingState" << std::endl;
}

