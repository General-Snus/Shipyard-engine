#include "PersistentSystems.pch.h"
#include "../PhysXInterpeter.h"

#include "PxConfig.h"
#include "PxPhysics.h"
#include "PxPhysicsAPI.h" 
#include <Engine/AssetManager/Objects/BaseAssets/MeshAsset.h>
#include <Tools/Utilities/Math.hpp>
#include <vector>

using namespace physx;

static PxDefaultAllocator gAllocator;
static PxDefaultErrorCallback gErrorCallback;
static PxFoundation* gFoundation = NULL;
static PxPhysics* gPhysics = NULL;
static PxDefaultCpuDispatcher* gDispatcher = NULL;
static PxScene* gScene = NULL;
static PxMaterial* gMaterial = NULL;
static PxPvd* gPvd = NULL;

static PxReal stackZ = 10.0f;
#pragma region hide this shit
void threeaxisrot(float r11,float r12,float r21,float r31,float r32,float res[])
{
	res[0] = atan2(r31,r32);
	res[1] = asin(r21);
	res[2] = atan2(r11,r12);
}

void quaternion2Euler(const physx::PxQuat& q,float res[])
{
	threeaxisrot(-2 * (q.y * q.z - q.w * q.x),
		q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z,
		2 * (q.x * q.z + q.w * q.y),
		-2 * (q.x * q.y - q.w * q.z),
		q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z,
		res);
}
#pragma endregion


static void createStack(const PxTransform& t,PxU32 size,PxReal halfExtent,PxRigidDynamic* dataPtr = nullptr)
{
	PxShape* shape = gPhysics->createShape(PxBoxGeometry(halfExtent,halfExtent,halfExtent),*gMaterial);
	for(PxU32 i = 0; i < size; i++)
	{
		for(PxU32 j = 0; j < size - i; j++)
		{
			PxTransform localTm(PxVec3(PxReal(j * 2) - PxReal(size - i),PxReal(i * 2 + 1),0) * halfExtent);
			PxRigidDynamic* body = gPhysics->createRigidDynamic(t.transform(localTm));
			body->attachShape(*shape);
			PxRigidBodyExt::updateMassAndInertia(*body,10.0f);
			gScene->addActor(*body);

			dataPtr = body;
		}
	}
	shape->release();
}

int Shipyard_PhysX::InitializePhysx()
{
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION,gAllocator,gErrorCallback);


	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION,*gFoundation,PxTolerancesScale(),true,gPvd);
	gMaterial = gPhysics->createMaterial(0.5f,0.5f,0.6f);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f,-9.81f,0.0f);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;

	gScene = gPhysics->createScene(sceneDesc);
	//gScene->setVisualizationParameter(PxVisualizationParameter::eSCALE,1.0f);
	//gScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES,true);



	PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics,PxPlane(0,1,0,0),*gMaterial);
	gScene->addActor(*groundPlane);


	return 0;
}
void Shipyard_PhysX::Update(float deltaTime)
{
	gScene->simulate(deltaTime);
	gScene->fetchResults(true);
}

void Shipyard_PhysX::Render()
{
	const PxRenderBuffer& rb = gScene->getRenderBuffer();
	for(PxU32 i = 0; i < rb.getNbLines(); i++)
	{
		const PxDebugLine& line = rb.getLines()[i];
		const Vector3f Vpos1 = {line.pos0.x,line.pos0.y,line.pos0.z};
		const Vector3f Vpos2 = {line.pos1.x,line.pos1.y,line.pos1.z};

		DebugDrawer::Get().AddDebugLine(Vpos1,Vpos2,Vector3f(),Timer::GetInstance().GetDeltaTime());
	}
}


void Shipyard_PhysX::ShutdownPhysx()
{
	PX_RELEASE(gScene);
	PX_RELEASE(gDispatcher);
	PX_RELEASE(gPhysics);
	if(gPvd)
	{
		PxPvdTransport* transport = gPvd->getTransport();
		gPvd->release();	gPvd = NULL;
		PX_RELEASE(transport);
	}
	PX_RELEASE(gFoundation);

	printf("SnippetHelloWorld done.\n");
}

PxPhysics* Shipyard_PhysX::GetPhysicsWorld()
{
	return gPhysics;
}

PxScene* Shipyard_PhysX::GetScene()
{
	return gScene;
}

physx::PxConvexMesh* Shipyard_PhysX::CookMesh(std::shared_ptr<Mesh> myToBeCookedMesh)
{
	//WAS WORKING HERE

	PxConvexMeshDesc convexDesc;
	convexDesc.points.count = 5;
	convexDesc.points.stride = sizeof(PxVec3);
	convexDesc.points.data = convexVerts;
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;
	return nullptr;
}
//void Shipyard_UserErrorCallback::reportError(physx::PxErrorCode::Enum code,const char* message,const char* file,int line)
//{
//	std::cout << "PhysX Error : (" << (int)code << ") " << message << " in " << file << " at line " << line << std::endl;
//	// error processing implementation 
//}