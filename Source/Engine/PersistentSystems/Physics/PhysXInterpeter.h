#pragma once  
#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>
#include <Tools/Utilities/System/SingletonTemplate.h> 


namespace physx
{
	class PxPhysics;
	class PxDefaultErrorCallback;
	class PxDefaultAllocator;
	class PxFoundation;
	class PxPvd;
	class PxScene;
	class PxActor;
}


struct TransformData
{
	Vector3f position;
	Vector3f rotation;
};

class Shipyard_PhysX : public Singleton<Shipyard_PhysX>
{
public:
	friend class Singleton<Shipyard_PhysX>;
	int InitializePhysx();
	void Update();
	void Render();

	void ShutdownPhysx();

	bool  AssignActor(TransformData* dataPtr);
	physx::PxActor* GetActor(int aActorID);
	TransformData* GetActorData(int aActorID);

	physx::PxScene* GetScene();

private:
	/*static physx::PxPhysics* mPhysics;
	static physx::PxDefaultErrorCallback* gDefaultErrorCallback;
	static physx::PxDefaultAllocator* gDefaultAllocatorCallback;
	static physx::PxFoundation* mFoundation;
	static physx::PxPvd* mPvd;*/
};

//class Shipyard_PhysXAllocatorCallback : public physx::PxDefaultAllocator
//{
//public:
//	virtual ~Shipyard_PhysXAllocatorCallback() {}
//	virtual void* allocate(size_t size, const char* typeName, const char* filename,
//		int line) = 0;
//	virtual void deallocate(void* ptr) = 0;
//};
//
//class Shipyard_UserErrorCallback : public physx::PxDefaultErrorCallback
//{
//public:
//	virtual void reportError(physx::PxErrorCode::Enum code,const char* message,const char* file,int line);
//};