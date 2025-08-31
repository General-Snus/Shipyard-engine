#pragma once
#include "Engine/AssetManager/ComponentSystem/Components/Network/NetworkSync.h"
#include "NetMessage/NetMessage.h"
#include "NetworkStructs.h"
#include <Tools/Utilities/DataStructures/QuadTree.h>
#include <utility>
#include <array>

class NetworkRunner;

class ReplicationLayer
{
	using IdToObject = std::pair<NetworkedId, NetworkObject>;
public:
	ReplicationLayer();
	//Fixed network update contains the component updates in the server
	void fixedNetworkUpdate(NetworkRunner& runner);
	void server_fixedNetworkUpdate(NetworkRunner& runner);
	// Transform Example: looks at transform, sets networktransform to same and copies data, ensures that its object exists
	//It then sends the udp messages of its new information 

	// looks at its network transform data and applies interpolation on it
	void client_fixedNetworkUpdate(const NetworkRunner& runner) const;
	void client_ReadIncoming(const NetworkRunner& runner);

	//If the id to the object does not exist then we need to create it, in case that we are the server
	void receiveMessage(const NetMessage&);

	//Registers a object to the map, made on the server 
	bool registerObject(const NetworkRunner& runner, const NetworkObject& object);
	bool unRegisterObject(const NetworkRunner& runner, const NetworkObject& object);
	void RegisterPlayerAOI(Networking::AreaOfInterest aoi);
	Networking::AreaOfInterest AOI() const;

	void Close();
private:
	//The position that you expect your replication layer to cull for you from. Does not apply to the server
	Networking::AreaOfInterest aoi = Networking::AreaOfInterest(Vector3f::zero(), defaultAOIRange);
	constexpr static float  defaultAOIRange = 5.0f;
	Networking::SyncRates ShouldSpacialCull(Vector3f positionOfObject, Networking::AreaOfInterest remoteZone) const;
	constexpr static std::array<float, Cast<int>(Networking::SyncRates::count)> syncTimes = {
		-1.0f,
		1.f / 20.f,
		1.f / 100.f
	};

	struct cullerPosition
	{
		NetworkedId id;
		float frequencyInverse = 60.0f;
		float timeSinceLastUpdate = 0.0f;
		inline float GetFrequency() const
		{
			return 1.0f / frequencyInverse;
		}
	};
	QuadTree<cullerPosition> spacialFrequencyCulling;

	//Suboptimal solution
	std::unordered_map<NetworkedId, NetworkObject> idToObjectMap;

	//Optimal solution
	// Explainer: We would want to have it connected per component type so that we can use
	//std::unordered_map<NetworkedId,NetworkTransform> idToObjectMap;
	//std::unordered_map<NetworkedId,NetworkColor> idToObjectMap;
	//std::unordered_map<NetworkedId,NetworkAnyThingElse> idToObjectMap;

};

