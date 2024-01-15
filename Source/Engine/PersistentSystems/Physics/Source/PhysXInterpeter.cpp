#include "PersistentSystems.pch.h" 
#include <Engine/AssetManager/Objects/BaseAssets/MeshAsset.h>
#include <pvd/PxPvdTransport.h>
#include <Tools/Utilities/Math.hpp>
#include "../PhysXInterpeter.h"
#include "cooking/PxCooking.h"  
#include "PxPhysics.h"
#include "PxPhysicsAPI.h" 

using namespace physx;

static PxDefaultAllocator gAllocator;
static PxDefaultErrorCallback gErrorCallback;
static PxFoundation* gFoundation = NULL;
static PxPhysics* gPhysics = NULL;
static PxDefaultCpuDispatcher* gDispatcher = NULL;
static PxScene* gScene = NULL;
static PxMaterial* gMaterial = NULL;
static PxPvd* gPvd = NULL;
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


int Shipyard_PhysX::InitializePhysx()
{
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION,gAllocator,gErrorCallback);

	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1",5425,10);
	gPvd->connect(*transport,PxPvdInstrumentationFlag::eALL);


	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION,*gFoundation,PxTolerancesScale(),true,gPvd);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f,-9.81f,0.0f);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;

	gScene = gPhysics->createScene(sceneDesc);
	gScene->setVisualizationParameter(PxVisualizationParameter::eSCALE,1.0f);
	gScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES,true);


	gMaterial = gPhysics->createMaterial(0.5f,0.5f,0.6f);
	//PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics,PxPlane(0,1,0,0),*gMaterial);
	//gScene->addActor(*groundPlane);
	return 0;
}

void Shipyard_PhysX::StartRead() const
{
	gScene->fetchResults(true);
}

void Shipyard_PhysX::EndRead(float deltaTime)
{
	gScene->simulate(deltaTime);
}

void Shipyard_PhysX::Render()
{
	const PxRenderBuffer& rb = gScene->getRenderBuffer();
	for (PxU32 i = 0; i < rb.getNbLines(); i++)
	{
		const PxDebugLine& line = rb.getLines()[i];
		const Vector3f Vpos1 = { line.pos0.x,line.pos0.y,line.pos0.z };
		const Vector3f Vpos2 = { line.pos1.x,line.pos1.y,line.pos1.z };

		DebugDrawer::Get().AddDebugLine(Vpos1,Vpos2,Vector3f(),Timer::GetInstance().GetDeltaTime());
	}
}


void Shipyard_PhysX::ShutdownPhysx()
{
	PX_RELEASE(gScene);
	PX_RELEASE(gDispatcher);
	PX_RELEASE(gPhysics);
	if (gPvd)
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



//Walter white component
physx::PxConvexMesh* Shipyard_PhysX::CookMesh(std::shared_ptr<Mesh> myToBeCookedMesh)
{
	const int vertCount = myToBeCookedMesh->Elements[0].NumVertices;
	PxVec3* convexVerts = new PxVec3[vertCount];

	PxConvexMeshDesc convexDesc;
	convexDesc.points.count = vertCount;
	convexDesc.points.stride = sizeof(PxVec3);
	convexDesc.points.data = convexVerts;
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX | PxConvexFlag::eDISABLE_MESH_VALIDATION | PxConvexFlag::eFAST_INERTIA_COMPUTATION;
	//PxDefaultMemoryOutputStream buf;
	//physx::PxConvexMesh* tst;
	//if(!PxCookConvexMesh(PxTolerancesScale(),convexDesc,buf))
	//{
	//	return NULL;
	//}
	//PxDefaultMemoryInputData* inpStream = new PxDefaultMemoryInputData(buf.getData(),buf.getSize());
	//tst = gPhysics->createConvexMesh(*inpStream);
	return NULL;
}
//void Shipyard_UserErrorCallback::reportError(physx::PxErrorCode::Enum code,const char* message,const char* file,int line)
//{
//	std::cout << "PhysX Error : (" << (int)code << ") " << message << " in " << file << " at line " << line << std::endl;
//	// error processing implementation 
//}
physx::PxMaterial* Shipyard_PhysX::GetDefaultMaterial()
{
	return gMaterial;
}