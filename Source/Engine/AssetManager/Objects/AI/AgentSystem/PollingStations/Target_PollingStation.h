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
		DataTuple(Vector3f a,GameObject b) :positionData(a),sourceObject(b){};
		Vector3f positionData;
		const GameObject sourceObject; // Is here purely to allow for is same check
	};

	explicit MultipleTargets_PollingStation(const std::vector<GameObject> aTarget);
	explicit MultipleTargets_PollingStation(const GameObject aTarget);
	std::vector<DataTuple> GetTargetPosition();
	void AddToTargetList(const GameObject aTarget);
private:
	std::vector<GameObject> targets;
};

