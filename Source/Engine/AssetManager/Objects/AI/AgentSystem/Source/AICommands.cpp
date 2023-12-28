#include "AssetManager.pch.h"
#include "../AICommands.h"


namespace GeneralizedAICommands {

	bool IsTargetInSight(GameObject input)
	{
		std::cout << "IsTargetInSight" << std::endl;
		//Is there other targets active?
		const Transform& myTransform = input.GetComponent<Transform>();
		const CombatComponent& myStats = input.GetComponent<CombatComponent>();

		Vector3f closestTarget = AIPollingManager::Get().GetStation<MultipleTargets_PollingStation>("Targets")->GetClosestTargetPosition(myTransform.GetPosition(),input);
		Vector3f direction = (closestTarget - myTransform.GetPosition()).GetNormalized();

		if(direction.Dot(myTransform.GetForward()) > cos(myStats.myAttackCone)) //45 degrees
		{
			return true;
		}

		return false;
	}

	bool IsTargetInRange(GameObject input)
	{
		std::cout << "IsTargetInRange" << std::endl;
		UNREFERENCED_PARAMETER(input);
		const Transform& myTransform = input.GetComponent<Transform>();

		Vector3f closestTarget = AIPollingManager::Get().GetStation<MultipleTargets_PollingStation>("Targets")->GetClosestTargetPosition(myTransform.GetPosition(),input);
		if((closestTarget - myTransform.GetPosition()).Length() < 10.0f)
		{
			return true;
		}
		return false;
	}

	bool IsTargetAlive(GameObject input)
	{
		std::cout << "IsTargetAlive" << std::endl;
		UNREFERENCED_PARAMETER(input);

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
		std::cout << "IsDead" << std::endl;
		UNREFERENCED_PARAMETER(input);

		if(const auto* stats = input.TryGetComponent<CombatComponent>())
		{
			return stats->myHealth < 1;
		}
		assert(false && "No health component found");
		return true;
	}

	bool IsHealthy(GameObject input)
	{
		std::cout << "IsHealthy" << std::endl;
		UNREFERENCED_PARAMETER(input);

		if(const auto* stats = input.TryGetComponent<CombatComponent>())
		{
			return stats->myHealth > 75;
		}
		assert(false && "No health component found");
		return true;
	}

	bool IsFighting(GameObject input)
	{
		std::cout << "IsFighting" << std::endl;
		UNREFERENCED_PARAMETER(input);
		return false;
	}

	bool IsWinning(GameObject input)
	{
		std::cout << "IsWinning" << std::endl;
		UNREFERENCED_PARAMETER(input);
		return false;
	}

	bool ShootAtTarget(GameObject input)
	{
		std::cout << "ShootAtTarget" << std::endl;
		UNREFERENCED_PARAMETER(input);

		if(const auto* stats = input.TryGetComponent<CombatComponent>())
		{
			stats->FireProjectile();
		}
		return false;
	}

	bool Retreat(GameObject input)
	{
		std::cout << "Retreat" << std::endl;
		UNREFERENCED_PARAMETER(input);

		//auto& physicsComponent = input.GetComponent<cPhysics_Kinematic>();
		//auto& transform = input.GetComponent<Transform>(); // You can use Get directly if you are sure it will exist, its faster and force safe code



		return false;
	}

	bool MoveToward(GameObject input)
	{
		std::cout << "MoveToward" << std::endl;
		UNREFERENCED_PARAMETER(input);

		auto& physicsComponent = input.GetComponent<cPhysics_Kinematic>();
		auto& transform = input.GetComponent<Transform>();

		SteeringBehaviour::Wander(&physicsComponent,transform.GetForward(),5.0f);

		return true;
	}

	bool AlignToTarget(GameObject input)
	{
		std::cout << "AlignToTarget" << std::endl;
		UNREFERENCED_PARAMETER(input);

		auto& physicsComponent = input.GetComponent<cPhysics_Kinematic>();
		auto& transform = input.GetComponent<Transform>(); // You can use Get directly if you are sure it will exist, its faster and force safe code

		Vector3f closestTarget = AIPollingManager::Get().GetStation<MultipleTargets_PollingStation>("Targets")->GetClosestTargetPosition(transform.GetPosition(),input);
		Vector3f direction = (closestTarget - transform.GetPosition()).GetNormalized();

		SteeringBehaviour::LookAt(&physicsComponent,direction,transform.GetForward(),5.0f);

		return true;
	}
}