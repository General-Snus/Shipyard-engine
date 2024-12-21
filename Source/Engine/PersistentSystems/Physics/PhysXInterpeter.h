#pragma once
#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>
#include <Tools/Utilities/System/SingletonTemplate.h>

namespace physx {
	class PxPhysics;
	class PxDefaultErrorCallback;
	class PxDefaultAllocator;
	class PxFoundation;
	class PxPvd;
	class PxScene;
	class PxActor;
	class PxRigidDynamic;
	class PxRenderBuffer;
	class PxConvexMesh;
	class PxDefaultCpuDispatcher;
	class PxRigidStatic;
	class PxMaterial;
	class PxTriangleMesh;
	class PxShape;
	class PxRigidActor;
	class PxGeometry;
} // namespace physx
class Mesh;

#include <Tools/Utilities/System/ServiceLocator.h>
#define Shipyard_PhysXInstance ServiceLocator::Instance().GetService<Shipyard_PhysX>()
class Shipyard_PhysX : public Singleton {
private:
	physx::PxDefaultAllocator* gAllocator;
	physx::PxDefaultErrorCallback* gErrorCallback;
	physx::PxFoundation* gFoundation{};
	physx::PxPhysics* gPhysics{};
	physx::PxDefaultCpuDispatcher* gDispatcher{};
	physx::PxScene* gScene{};
	physx::PxMaterial* gMaterial{};
	physx::PxPvd* gPvd{};

public:
	int InitializePhysx();
	void StartRead() const;
	void EndRead(float deltaTime);
	void Render();
	void ShutdownPhysx();
	void resetScene();

	// template<typename T>
	// static T* CookMesh(std::shared_ptr<Mesh> myToBeCookedMesh);

	physx::PxScene* GetScene();
	physx::PxPhysics* GetPhysicsWorld();
	physx::PxMaterial* GetDefaultMaterial();
	physx::PxShape* CreateShape(physx::PxRigidActor* actor,const physx::PxGeometry* geometry,const physx::PxMaterial* material = nullptr,
		uint8_t shapeFlags = 1 << 3 | 1 << 1 | 1 << 0);


};

// class Shipyard_PhysXAllocatorCallback : public physx::PxDefaultAllocator
//{
// public:
//	virtual ~Shipyard_PhysXAllocatorCallback() {}
//	virtual void* allocate(size_t size, const char* typeName, const char* filename,
//		int line) = 0;
//	virtual void deallocate(void* ptr) = 0;
// };
//
// class Shipyard_UserErrorCallback : public physx::PxDefaultErrorCallback
//{
// public:
//	virtual void reportError(physx::PxErrorCode::Enum code,const char* message,const char* file,int line);
// };
