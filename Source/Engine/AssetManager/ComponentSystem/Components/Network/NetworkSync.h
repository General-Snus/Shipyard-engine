#pragma once

#include <Engine/AssetManager/ComponentSystem/Component.h>

#include <filesystem>
#include <memory>
#include <unordered_set>
#include "Engine/PersistentSystems/Networking/NetworkStructs.h" 
#include "Tools/Utilities/LinearAlgebra/Quaternions.hpp"
#include "Tools/Utilities/Input/Mapper.hpp"

class NetworkRunner;
class NetworkObject : public Reflectable<NetworkObject>, public Component
{
public:
	reflectable(NetworkObject);
	NetworkObject() = delete;
	NetworkObject(const SY::UUID anOwnerId, GameObjectManager* aManager);
	NetworkObject(const SY::UUID anOwnerId, GameObjectManager* aManager, NetworkedId id);
	void Init() override;
	void Destroy() override;
	bool InspectorView() override;

	void RegisterAsPlayer() const;

	NetworkedId GetServerID() const
	{
		return uniqueNetId;
	}
	float SyncFrequencyInverse() const
	{
		return 1 / syncFrequency;
	}

	ServerTimePoint GetLastSyncTime() const
	{
		return clientUpdateTimePoint;
	}

	bool neverNotSync = false;

	bool ShouldSync(const NetworkRunner& runner) const;
	bool ShouldSync(const NetworkRunner& runner, float customSyncTime) const;
	void Synced(const ServerTimePoint& time);

	void DisperseNetMessage(const NetMessage& netMessageForIndividualobject);

	float syncFrequency = 20.0f;
protected:
	ServerTimePoint clientUpdateTimePoint;
	NetworkedId uniqueNetId;
};
REFL_AUTO(type(NetworkObject),field(syncFrequency))

class NetworkTransform : public Reflectable<NetworkTransform>, public NetworkObject
{
public:
	reflectable(NetworkTransform);
	NetworkTransform() = delete;
	NetworkTransform(const SY::UUID anOwnerId, GameObjectManager* aManager);
	bool InspectorView() override;
	void Init() override;

	Vector3f myPosition;
	Quaternionf    myQuaternion;
	Vector3f myScale = Vector3f(1, 1, 1);

	Vector3f translationInterpolation;
	Quaternionf    rotationInterpolation;
};
REFL_AUTO(type(NetworkTransform), field(myPosition), field(translationInterpolation))

class NetworkInputListener : public Reflectable<NetworkInputListener>, public NetworkObject
{
public:
	reflectable(NetworkInputListener);
	NetworkInputListener() = delete;
	NetworkInputListener(const SY::UUID anOwnerId, GameObjectManager* aManager);
	bool InspectorView() override;
	void Init() override;

	void AddKeyFunc(Keys key, Action action, bool allowLocalPrediction,MapperFunction func);
};
REFL_AUTO(type(NetworkInputListener))
