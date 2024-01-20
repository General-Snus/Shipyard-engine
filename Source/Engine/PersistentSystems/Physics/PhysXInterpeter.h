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
	class PxRigidDynamic;
	class PxRenderBuffer;
	class PxConvexMesh;
	class PxRigidStatic;
	class PxMaterial;
	class PxTriangleMesh;
}
class Mesh;

class Shipyard_PhysX : public Singleton<Shipyard_PhysX>
{
public:
	friend class Singleton<Shipyard_PhysX>;
	int InitializePhysx();
	void StartRead() const;
	void EndRead(float deltaTime);
	void Render();
	const physx::PxRenderBuffer& getRenderBuffer() const;

	void ShutdownPhysx();

	template<typename T>
	static T* CookMesh(std::shared_ptr<Mesh> myToBeCookedMesh);

	physx::PxScene* GetScene();
	physx::PxPhysics* GetPhysicsWorld();
	physx::PxMaterial* GetDefaultMaterial();
private:
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