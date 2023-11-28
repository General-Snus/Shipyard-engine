#include "AssetManager.pch.h"


Target_PollingStation::Target_PollingStation(const GameObject aTarget) : target(aTarget),PollingStation()
{
}

Vector3f Target_PollingStation::GetTargetPosition()
{
	return target.GetComponent<Transform>().GetPosition();
}

MultipleTargets_PollingStation::MultipleTargets_PollingStation(const std::vector<GameObject> aTarget)
{
	targets.insert(targets.end(),aTarget.begin(),aTarget.end());
}

MultipleTargets_PollingStation::MultipleTargets_PollingStation(const GameObject aTarget)
{
	targets.push_back(aTarget);
}

std::vector<MultipleTargets_PollingStation::DataTuple> MultipleTargets_PollingStation::GetTargetPosition()
{
	std::vector<DataTuple> returnVector;
	returnVector.reserve(targets.size());

	for(auto& g : targets)
	{ 
		returnVector.push_back(DataTuple(g.GetComponent<Transform>().GetPosition(),g));
	}

	//std::transform(targets.begin(),targets.end(),targets.begin(),
	//	[&returnVector](GameObject g)
	//	{
	//		returnVector.push_back(g.GetComponent<Transform>().GetPosition());
	//	});

	return returnVector;
}

void MultipleTargets_PollingStation::AddToTargetList(const GameObject aTarget)
{
	targets.push_back(aTarget);
}
