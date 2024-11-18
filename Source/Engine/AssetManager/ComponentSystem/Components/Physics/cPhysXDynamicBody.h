#pragma once 
#include <Engine/AssetManager/ComponentSystem/Component.h>
#include <Engine/PersistentSystems/Physics/PhysXInterpeter.h>


class cPhysXDynamicBody : public Component
{
public:
	MYLIB_REFLECTABLE();
	cPhysXDynamicBody(const SY::UUID anOwnerId,GameObjectManager* aManager);


	void Init() override;
	void Update() override; 
	void Destroy() override;
	void OnSiblingChanged(const std::type_info* SourceClass) override;
private:
	physx::PxRigidDynamic* data;
};

REFL_AUTO(type(cPhysXDynamicBody))
