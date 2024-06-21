#include "Engine/PersistentSystems/PersistentSystems.pch.h"

#include <Engine/AssetManager/ComponentSystem/Components/ActorSystem/CombatComponent.h>
#include <Engine/AssetManager/ComponentSystem/Components/Physics/cPhysics_Kinematic.h>
#include <Engine/AssetManager/ComponentSystem/Components/Transform.h>

#include <Engine/AssetManager/Objects/AI/AgentSystem/AIPollingManager.h>
#include <Engine/AssetManager/Objects/AI/AgentSystem/PollingStations/Target_PollingStation.h>
#include <Engine/AssetManager/Objects/AI/AgentSystem/SteeringBehaviour.h>

#include "../AICommands.h" 
#include <Engine/PersistentSystems/Physics/Raycast.h>
#include <Tools/Utilities/Math.hpp> 


namespace GeneralizedAICommands {

	bool IsTargetInSight(GameObject input)
	{
		input;
		//Is there other targets active?
		const Transform& myTransform = input.GetComponent<Transform>();
		const CombatComponent& myStats = input.GetComponent<CombatComponent>();

		Vector3f closestTarget = AIPollingManager::Get().GetStation<MultipleTargets_PollingStation>("Targets")->GetClosestTargetPosition(myTransform.GetPosition(),input);
		Vector3f direction = (closestTarget - myTransform.GetPosition()).GetNormalized();

		if(direction.Dot(myTransform.GetForward()) > cos(DEG_TO_RAD * myStats.myAttackCone)) //10 degrees artificial spread
		{
			return true;
		}

		return false;
	}

	bool IsTargetInRange(GameObject input)
	{
		input;
		const Transform& myTransform = input.GetComponent<Transform>();
		const CombatComponent& myStats = input.GetComponent<CombatComponent>();

		GameObject obj = AIPollingManager::Get().GetStation<MultipleTargets_PollingStation>("Targets")->GetClosestTarget(myTransform.GetPosition(),input);
		Vector3f closestTarget = obj.GetComponent<Transform>().GetPosition();
		const float distance = (closestTarget - myTransform.GetPosition()).Length();


		if(distance < myStats.myVisionRange)
		{
			Physics::RaycastHit hit;
			Vector3f color = {1,0,0};
			const Vector3f direction = (closestTarget - myTransform.GetPosition()).GetNormalized();

			if(Physics::Raycast(myTransform.GetPosition(),direction,hit,input,
				(Layer)((int)Layer::Entities | (int)Layer::Default)))
			{
				if(hit.objectHit.GetID() == obj.GetID())
				{
					color = {0,1,0};
					DebugDrawer::Get().AddDebugLine(myTransform.GetPosition(),closestTarget,color,0.01f);
					return true;
				}
			}
			DebugDrawer::Get().AddDebugLine(myTransform.GetPosition(),closestTarget,color,0.01f);
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

	bool DeathSpin(GameObject input)
	{
		(input);
		auto& physicsComponent = input.GetComponent<cPhysics_Kinematic>();
		physicsComponent.ph_Angular_velocity = {0,500,0};
		physicsComponent.ph_velocity = {0,0,0};
		return true;
	}

	bool IsFullyHealed(GameObject input)
	{
		(input);

		if(const auto* stats = input.TryGetComponent<CombatComponent>())
		{
			return stats->myHealth >= 100;
		}
		assert(false && "No health component found");
		return true;
	}

	bool IsHealthy(GameObject input)
	{
		(input);

		if(const auto* stats = input.TryGetComponent<CombatComponent>())
		{
			return stats->myHealth > 25;
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
		SteeringBehaviour::DampenVelocity(&physicsComponent,10);
		if(auto* stats = input.TryGetComponent<CombatComponent>())
		{
			stats->FireProjectile();
		}
		return false;
	}

	bool Retreat(GameObject input)
	{
		(input);

		auto& physicsComponent = input.GetComponent<cPhysics_Kinematic>();
		auto& transform = input.GetComponent<Transform>();

		Vector3f closestWell = AIPollingManager::Get().GetStation<MultipleTargets_PollingStation>("Healing")->GetClosestTargetPosition(transform.GetPosition());
		SteeringBehaviour::DampenVelocity(&physicsComponent);
		SteeringBehaviour::LookAt(&physicsComponent,physicsComponent.ph_velocity,transform.GetForward(),5.0f);
		if(SteeringBehaviour::Arrive(&physicsComponent,closestWell,transform.GetPosition(),1.0f,3.0f))
		{
			input.GetComponent<CombatComponent>().Healing();
		}

		return true;
	}
	bool MoveFreely(GameObject input)
	{
		auto& physicsComponent = input.GetComponent<cPhysics_Kinematic>();
		auto& transform = input.GetComponent<Transform>();

		SteeringBehaviour::LookAt(&physicsComponent,physicsComponent.ph_velocity,transform.GetForward(),50.0f);
		SteeringBehaviour::Wander(&physicsComponent,transform.GetForward(),5.f);
		SteeringBehaviour::DampenVelocity(&physicsComponent);

		return true;
	}

	bool MoveToward(GameObject input)
	{
		(input);

		auto& physicsComponent = input.GetComponent<cPhysics_Kinematic>();
		auto& transform = input.GetComponent<Transform>();


		SteeringBehaviour::DampenVelocity(&physicsComponent);
		SteeringBehaviour::LookAt(&physicsComponent,physicsComponent.ph_velocity,transform.GetForward(),5.0f);
		Vector3f position = AIPollingManager::Get().GetStation<MultipleTargets_PollingStation>("Targets")->GetClosestTargetPosition(transform.GetPosition(),input);

		SteeringBehaviour::Arrive(&physicsComponent,position,transform.GetPosition());
		return true;
	}

	bool AlignToTarget(GameObject input)
	{
		(input);

		auto& physicsComponent = input.GetComponent<cPhysics_Kinematic>();
		auto& transform = input.GetComponent<Transform>();

		Vector3f closestTarget = AIPollingManager::Get().GetStation<MultipleTargets_PollingStation>("Targets")->GetClosestTargetPosition(transform.GetPosition(),input);
		Vector3f direction = (closestTarget - transform.GetPosition()).GetNormalized();
		SteeringBehaviour::DampenVelocity(&physicsComponent,5);
		SteeringBehaviour::LookAt(&physicsComponent,direction,transform.GetForward(),5.0f);

		return true;
	}
}