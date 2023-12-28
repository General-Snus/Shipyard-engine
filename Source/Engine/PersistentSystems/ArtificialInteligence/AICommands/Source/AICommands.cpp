#include "PersistentSystems.pch.h"
#include <Engine/AssetManager/ComponentSystem/Components/ActorSystem/CombatComponent.h>
#include <Engine/AssetManager/ComponentSystem/Components/Physics/cPhysics_Kinematic.h>
#include <Engine/AssetManager/ComponentSystem/Components/Transform.h>

#include <Engine/AssetManager/Objects/AI/AgentSystem/AIPollingManager.h>
#include <Engine/AssetManager/Objects/AI/AgentSystem/PollingStations/Target_PollingStation.h>
#include <Engine/AssetManager/Objects/AI/AgentSystem/SteeringBehaviour.h>

#include "../AICommands.h" 
#include <Tools/Utilities/Math.hpp> 


namespace GeneralizedAICommands {

	bool IsTargetInSight(GameObject input)
	{
		input;
		////Is there other targets active?
		//const Transform& myTransform = input.GetComponent<Transform>();
		//const CombatComponent& myStats = input.GetComponent<CombatComponent>();

		//Vector3f closestTarget = AIPollingManager::Get().GetStation<MultipleTargets_PollingStation>("Targets")->GetClosestTargetPosition(myTransform.GetPosition(),input);
		//Vector3f direction = (closestTarget - myTransform.GetPosition()).GetNormalized();

		//if(direction.Dot(myTransform.GetForward()) > cos(DEG_TO_RAD * myStats.myAttackCone)) //45 degrees
		//{
		//	return true;
		//}

		return false;
	}

	bool IsTargetInRange(GameObject input)
	{
		input;
		const Transform& myTransform = input.GetComponent<Transform>();
		const CombatComponent& myStats = input.GetComponent<CombatComponent>();

		Vector3f closestTarget = AIPollingManager::Get().GetStation<MultipleTargets_PollingStation>("Targets")->GetClosestTargetPosition(myTransform.GetPosition(),input);
		if((closestTarget - myTransform.GetPosition()).Length() < myStats.myVisionRange)
		{
			DebugDrawer::Get().AddDebugLine(myTransform.GetPosition(),closestTarget,{0,1,1},0.5f);
			return true;
		}
		return false;
	}

	bool IsTargetAlive(GameObject input)
	{
		(input);

		const Transform& myTransform = input.GetComponent<Transform>();
		GameObject target = AIPollingManager::Get().GetStation<MultipleTargets_PollingStation>
			("Targets")->GetClosestTarget(myTransform.GetPosition(),input);

		if(const auto* stats = target.TryGetComponent<CombatComponent>())
		{
			return stats->myHealth > 0;
		}

		return false;
	}

	bool IsDead(GameObject input)
	{
		(input);

		if(const auto* stats = input.TryGetComponent<CombatComponent>())
		{
			return stats->myHealth < 1;
		}
		assert(false && "No health component found");
		return true;
	}

	bool IsHealthy(GameObject input)
	{
		(input);

		if(const auto* stats = input.TryGetComponent<CombatComponent>())
		{
			return stats->myHealth > 75;
		}
		assert(false && "No health component found");
		return true;
	}

	bool IsFighting(GameObject input)
	{
		(input);
		return false;
	}

	bool IsWinning(GameObject input)
	{
		(input);
		return false;
	}

	bool ShootAtTarget(GameObject input)
	{
		(input);
		auto& physicsComponent = input.GetComponent<cPhysics_Kinematic>();
		SteeringBehaviour::DampenVelocity(&physicsComponent);
		if(auto* stats = input.TryGetComponent<CombatComponent>())
		{
			stats->FireProjectile();
		}
		return false;
	}

	bool Retreat(GameObject input)
	{
		(input);

		//auto& physicsComponent = input.GetComponent<cPhysics_Kinematic>();
		//auto& transform = input.GetComponent<Transform>(); // You can use Get directly if you are sure it will exist, its faster and force safe code



		return false;
	}

	bool MoveToward(GameObject input)
	{
		(input);

		auto& physicsComponent = input.GetComponent<cPhysics_Kinematic>();
		auto& transform = input.GetComponent<Transform>();

		Vector3f newPosition = SteeringBehaviour::SetPositionInBounds(transform.GetPosition(),50.0f);
		transform.SetPosition(newPosition);
		SteeringBehaviour::DampenVelocity(&physicsComponent);
		SteeringBehaviour::LookAt(&physicsComponent,physicsComponent.ph_velocity,transform.GetForward(),5.0f);
		SteeringBehaviour::Wander(&physicsComponent,transform.GetForward(),5.0f);

		return true;
	}

	bool AlignToTarget(GameObject input)
	{
		(input);

		auto& physicsComponent = input.GetComponent<cPhysics_Kinematic>();
		auto& transform = input.GetComponent<Transform>(); // You can use Get directly if you are sure it will exist, its faster and force safe code

		Vector3f closestTarget = AIPollingManager::Get().GetStation<MultipleTargets_PollingStation>("Targets")->GetClosestTargetPosition(transform.GetPosition(),input);
		Vector3f direction = (closestTarget - transform.GetPosition()).GetNormalized();
		SteeringBehaviour::DampenVelocity(&physicsComponent);
		SteeringBehaviour::LookAt(&physicsComponent,direction,transform.GetForward(),5.0f);

		return true;
	}
}