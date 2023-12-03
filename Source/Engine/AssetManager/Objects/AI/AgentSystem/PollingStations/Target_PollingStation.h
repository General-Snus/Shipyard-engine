#pragma once
#include "PollingStation.h"  

class Target_PollingStation : public PollingStation
{
public:
	explicit Target_PollingStation(const GameObject aTarget);
	Vector3f GetTargetPosition();
private:
	GameObject target;
};

class MultipleTargets_PollingStation : public PollingStation
{
public:
	struct DataTuple
	{
		DataTuple(Vector3f a,SY::UUID b) :positionData(a),sourceObject(b){};
		Vector3f positionData;
		const SY::UUID sourceObject; // Is here purely to allow for is same check
	};

	explicit MultipleTargets_PollingStation(const std::vector<GameObject> aTarget);
	explicit MultipleTargets_PollingStation(const GameObject aTarget);

	void AddToTargetList(const GameObject aTarget);

	std::vector<DataTuple> GetTargetPosition();
	Vector3f GetClosestTargetPosition(Vector3f myPosition);

	Vector3f GetAverageVelocity();
	Vector3f GetAverageVelocityWithinCircle(Vector3f myPosition,float radius);

	//Returns all agents in circle
	std::vector<DataTuple> GetTargetsWithinCircle(Vector3f position,float radius);


	//Gets all agents in circle and returns the center of mass of those agents
	Vector3f GetCoMWithinCircle(Vector3f position,float radius,int& EntitiesInCircle);
	
	//returns the center of mass of all agents
	Vector3f GetCoM();
private:
	std::vector<GameObject> targets;
};

