#include "AssetManager.pch.h"
#include "../AICommands.h"


namespace GeneralizedAICommands {

	bool IsTargetInSight(GameObject input)
	{
		//Is there other targets active?
		const Transform& myTransform = input.GetComponent<Transform>();
		Vector3f closestTarget = AIPollingManager::Get().GetStation<MultipleTargets_PollingStation>("Targets")->GetClosestTargetPosition(myTransform.GetPosition(),input);
		closestTarget;

		return false;
	}

	bool IsTargetInRange(GameObject input)
	{
		UNREFERENCED_PARAMETER(input);
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