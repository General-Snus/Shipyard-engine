#include "AssetManager.pch.h"
#include "../AICommands.h"


namespace GeneralizedAICommands {

	bool IsTargetInSight(GameObject input)
	{
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
		UNREFERENCED_PARAMETER(input);
		return false;
	}

	bool IsDead(GameObject input)
	{
		UNREFERENCED_PARAMETER(input);
		return true;
	}

	bool IsHealthy(GameObject input)
	{
		UNREFERENCED_PARAMETER(input);
		return false;
	}

	bool IsFighting(GameObject input)
	{
		UNREFERENCED_PARAMETER(input);
		return false;
	}

	bool IsWinning(GameObject input)
	{
		UNREFERENCED_PARAMETER(input);
		return false;
	}

	bool ShootAtTarget(GameObject input)
	{
		UNREFERENCED_PARAMETER(input);
		return false;
	}

	bool Retreat(GameObject input)
	{
		UNREFERENCED_PARAMETER(input);
		return false;
	}

	bool MoveToward(GameObject input)
	{
		UNREFERENCED_PARAMETER(input);
		return false;
	}

	bool AlignToTarget(GameObject input)
	{
		UNREFERENCED_PARAMETER(input);
		return false;
	}
}