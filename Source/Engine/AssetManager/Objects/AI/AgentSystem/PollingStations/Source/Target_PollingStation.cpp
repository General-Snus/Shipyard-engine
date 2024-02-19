#include "AssetManager.pch.h"

Target_PollingStation::Target_PollingStation(const GameObject aTarget) : target(aTarget),PollingStation()
{
}

Vector3f Target_PollingStation::GetTargetPosition()
{
	return target.GetComponent<Transform>().GetPosition();
}

MultipleTargets_PollingStation::MultipleTargets_PollingStation(const std::vector<GameObject>& aTarget)
{
	targets.insert(targets.end(),aTarget.begin(),aTarget.end());
}

MultipleTargets_PollingStation::MultipleTargets_PollingStation(const GameObject aTarget)
{
	targets.push_back(aTarget);
}

std::vector<MultipleTargets_PollingStation::DataTuple> MultipleTargets_PollingStation::GetTargetPosition()
{
	OPTICK_EVENT();
	std::vector<DataTuple> returnVector;
	returnVector.reserve(targets.size());

	for (auto& g : targets)
	{
		returnVector.push_back(DataTuple(g.GetComponent<Transform>().GetPosition(),g.GetID()));
	}
	return returnVector;
}

std::vector<MultipleTargets_PollingStation::DataTuple> MultipleTargets_PollingStation::GetTargetPosition(GameObject filter)
{
	OPTICK_EVENT();
	std::vector<DataTuple> returnVector;
	returnVector.reserve(targets.size());
	SY::UUID filterID = filter.GetID();
	for (auto& g : targets)
	{
		if (filterID == g.GetID())
		{
			continue;
		}

		returnVector.push_back(DataTuple(g.GetComponent<Transform>().GetPosition(),g.GetID()));
	}
	return returnVector;
}

Vector3f MultipleTargets_PollingStation::GetClosestTargetPosition(Vector3f myPosition)
{
	OPTICK_EVENT();
	float closestDistance = FLT_MAX;
	Vector3f closestPosition = myPosition;

	for (auto& g : targets)
	{
		const float dist = (g.GetComponent<Transform>().GetPosition() - myPosition).LengthSqr();
		if (dist < closestDistance)
		{
			closestDistance = dist;
			closestPosition = g.GetComponent<Transform>().GetPosition();
		}
	}
	return closestPosition;
}

Vector3f MultipleTargets_PollingStation::GetClosestTargetPosition(Vector3f myPosition,GameObject filter)
{
	OPTICK_EVENT();
	float closestDistance = FLT_MAX;
	Vector3f closestPosition = myPosition;
	SY::UUID filterID = filter.GetID();

	for (auto& g : targets)
	{
		if (filterID == g.GetID())
		{
			continue;
		}
		const float dist = (g.GetComponent<Transform>().GetPosition() - myPosition).LengthSqr();
		if (dist < closestDistance)
		{
			closestDistance = dist;
			closestPosition = g.GetComponent<Transform>().GetPosition();
		}
	}
	return closestPosition;
}

GameObject MultipleTargets_PollingStation::GetClosestTarget(Vector3f myPosition)
{
	float closestDistance = FLT_MAX;
	GameObject closestObj;

	for (auto& g : targets)
	{
		const float dist = (g.GetComponent<Transform>().GetPosition() - myPosition).LengthSqr();
		if (dist < closestDistance)
		{
			closestDistance = dist;
			closestObj = g;
		}
	}
	return closestObj;
}

GameObject MultipleTargets_PollingStation::GetClosestTarget(Vector3f myPosition,GameObject filter)
{
	float closestDistance = FLT_MAX;
	GameObject closestObj;
	SY::UUID filterID = filter.GetID();

	for (auto& g : targets)
	{
		if (filterID == g.GetID())
		{
			continue;
		}
		const float dist = (g.GetComponent<Transform>().GetPosition() - myPosition).LengthSqr();
		if (dist < closestDistance)
		{
			closestDistance = dist;
			closestObj = g;
		}
	}
	return closestObj;
}

Vector3f MultipleTargets_PollingStation::GetAverageVelocity()
{
	OPTICK_EVENT();
	auto totalVelocity = Vector3f();
	for (auto& g : targets)
	{
		totalVelocity += g.GetComponent<cPhysics_Kinematic>().ph_velocity;
	}

	totalVelocity /= (float)targets.size();
	return totalVelocity;
}

Vector3f MultipleTargets_PollingStation::GetAverageVelocityWithinCircle(Vector3f position,float radius)
{
	OPTICK_EVENT();
	if (targets.empty())
	{
		return Vector3f();
	}

	auto totalVelocity = Vector3f();
	for (auto& g : targets)
	{
		if ((g.GetComponent<Transform>().GetPosition() - position).Length() < radius)
		{
			totalVelocity += g.GetComponent<cPhysics_Kinematic>().ph_velocity;
		}
	}

	totalVelocity /= (float)targets.size();
	return totalVelocity;
}

std::vector<MultipleTargets_PollingStation::DataTuple> MultipleTargets_PollingStation::GetTargetsWithinCircle(Vector3f position,float radius)
{
	OPTICK_EVENT();
	std::vector<DataTuple> returnVector;
	returnVector.reserve(targets.size()); //Better to over reserve?

	for (auto& g : targets)
	{
		Vector3f targetPosition = g.GetComponent<Transform>().GetPosition();

		if ((targetPosition - position).Length() < radius)
		{
			returnVector.push_back(DataTuple(targetPosition,g.GetID()));
		}
	}

	return returnVector;
}

Vector3f MultipleTargets_PollingStation::GetCoMWithinCircle(Vector3f position,float radius,int& EntitiesInCircle)
{
	OPTICK_EVENT();
	std::vector<DataTuple> returnVector = GetTargetsWithinCircle(position,radius);
	EntitiesInCircle = (int)returnVector.size();

	if (returnVector.empty())
	{//Check for this senario using entitiesInCircle
		return Vector3f();
	}

	Vector3f totalPosition = Vector3f();

	for (const auto& tuple : returnVector)
	{
		totalPosition += tuple.positionData;
	}

	return totalPosition / (float)returnVector.size();
}

Vector3f MultipleTargets_PollingStation::GetCoM()
{
	OPTICK_EVENT();
	Vector3f totalPosition = Vector3f();
	for (auto& g : targets)
	{
		totalPosition += g.GetComponent<Transform>().GetPosition();
	}
	return totalPosition / (float)targets.size();
}

void MultipleTargets_PollingStation::AddToTargetList(const GameObject aTarget)
{
	OPTICK_EVENT();
	targets.push_back(aTarget);
}

Vector3f MultipleTargets_PollingStation::GetClosestPositionOnCollider(Vector3f position)
{
	float closestDistance = FLT_MAX;
	Vector3f closestPosition = position;
	for (auto& g : targets)
	{
		const Vector3f coll = g.GetComponent<cCollider>().GetClosestPosition(position);
		const float dist = (coll - position).LengthSqr();
		if (dist < closestDistance)
		{
			closestDistance = dist;
			closestPosition = coll;
		}
	}
	return closestPosition;
}

Vector3f MultipleTargets_PollingStation::GetClosestPositionOnCollider(Vector3f position,Vector3f& normal)
{
	float closestDistance = FLT_MAX;
	Vector3f closestPosition = position;
	for (auto& g : targets)
	{
		const auto& collider = g.GetComponent<cCollider>();
		const Vector3f coll = collider.GetClosestPosition(position);
		const float dist = (coll - position).LengthSqr();
		if (dist < closestDistance)
		{
			closestDistance = dist;
			closestPosition = coll;
			normal = collider.GetNormalToward(position);
		}
	}
	return closestPosition;
}
