#pragma once 
#include <Engine/AssetManager/ComponentSystem/Component.h>
#include <Engine/PersistentSystems/Physics/PhysXInterpeter.h>
class cPhysXDynamicBody : public Component
{
public:
	cPhysXDynamicBody(const SY::UUID anOwnerID);
	cPhysXDynamicBody(const SY::UUID anOwnerID,TransformData* data);

	void SetId(TransformData* id);
	TransformData* GetId();
	void Init() override;
	void Update() override;
	void Render() override;
private:
	TransformData* data;
};

