#pragma once 
#include <Engine/AssetManager/ComponentSystem/Component.h>
#include <Engine/PersistentSystems/Physics/PhysXInterpeter.h>


class cPhysXDynamicBody : public Component
{
public:
	cPhysXDynamicBody(const SY::UUID anOwnerID);


	void Init() override;
	void Update() override;
	void Render() override;
	void Destroy() override;
	void OnSiblingChanged(const std::type_info* SourceClass) override;
private:
	physx::PxRigidDynamic* data;
};

