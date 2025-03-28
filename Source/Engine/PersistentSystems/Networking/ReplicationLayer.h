#pragma once
#include "Engine/AssetManager/ComponentSystem/Components/Network/NetworkSync.h"
#include <unordered_map>
#include <utility>
#include "NetMessage/NetMessage.h"
#include "NetworkStructs.h"

class NetworkRunner;

class ReplicationLayer
{ 
	using IdToObject = std::pair<NetworkedId,NetworkObject>;
public:

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
	bool registerObject(const NetworkRunner & runner,const NetworkObject & object);
	bool unRegisterObject(const NetworkRunner & runner,const NetworkObject & object);


	void Close();

	//Suboptimal solution
	std::unordered_map<NetworkedId,NetworkObject> idToObjectMap;

	//Optimal solution
	// Explainer: We would want to have it connected per component type so that we can use
	//std::unordered_map<NetworkedId,NetworkTransform> idToObjectMap;
	//std::unordered_map<NetworkedId,NetworkColor> idToObjectMap;
	//std::unordered_map<NetworkedId,NetworkAnyThingElse> idToObjectMap;

};

