#include "PersistentSystems.pch.h"
#include "../PhysXInterpeter.h"

#include "PxConfig.h"
#include "PxPhysics.h"
#include "PxPhysicsAPI.h"
#include "PxScene.h"
#include "snippetcommon/SnippetPrint.h"
#include "snippetcommon/SnippetPVD.h"
#include "snippetrender/SnippetCamera.h" 
#include "snippetutils/SnippetUtils.h"
#include <Tools/Utilities/Math.hpp>
#include <vector>

using namespace physx;

extern void initPhysics(bool interactive);
extern void stepPhysics(bool interactive);
extern void cleanupPhysics(bool interactive);
extern void keyPress(unsigned char key,const PxTransform& camera);

static PxDefaultAllocator		gAllocator;
static PxDefaultErrorCallback	gErrorCallback;
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

static PxRigidDynamic* createDynamic(const PxTransform& t,const PxGeometry& geometry,const PxVec3& velocity = PxVec3(0),TransformData* dataPtr = nullptr)
{
	PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics,t,geometry,*gMaterial,10.0f);
	dynamic->setAngularDamping(0.5f);
	dynamic->setLinearVelocity(velocity);
	gScene->addActor(*dynamic);
	dynamic->userData = new TransformData();
	dynamic->userData = dataPtr;

	return dynamic;
}

static void createStack(const PxTransform& t,PxU32 size,PxReal halfExtent,TransformData* dataPtr = nullptr)
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

			dataPtr = (TransformData*)body->userData;
		}
	}
	shape->release();
}

int Shipyard_PhysX::InitializePhysx()
{
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION,gAllocator,gErrorCallback);


	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION,*gFoundation,PxTolerancesScale(),true,gPvd);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f,-9.81f,0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	gScene = gPhysics->createScene(sceneDesc);

	gMaterial = gPhysics->createMaterial(0.5f,0.5f,0.6f);

	PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics,PxPlane(0,1,0,0),*gMaterial);
	gScene->addActor(*groundPlane);

	for(PxU32 i = 0; i < 5; i++)
		createStack(PxTransform(PxVec3(0,0,stackZ -= 10.0f)),10,2.0f);

	if(!true)
		createDynamic(PxTransform(PxVec3(0,40,100)),PxSphereGeometry(10),PxVec3(0,-50,-100));

	return 0;
}
void stepPhysics(bool /*interactive*/)
{
	gScene->simulate(1.0f / 60.0f);
	gScene->fetchResults(true);
}

void cleanupPhysics(bool /*interactive*/)
{

}
void Shipyard_PhysX::Update()
{
}

void Shipyard_PhysX::Render()
{
	stepPhysics(true);
	PxScene* scene;
	PxGetPhysics().getScenes(&scene,1);
	PxU32 nbActors = scene->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
	if(nbActors)
	{
		std::vector<PxRigidActor*> actors(nbActors);
		scene->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC,reinterpret_cast<PxActor**>(&actors[0]),nbActors);


		PxShape* shapes[6000];
		for(PxU32 i = 0; i < nbActors; i++)
		{
			const PxU32 nbShapes = actors[i]->getNbShapes();
			PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
			actors[i]->getShapes(shapes,nbShapes);

			bool sleeping;
			sleeping = actors[i]->is<PxRigidDynamic>() ? actors[i]->is<PxRigidDynamic>()->isSleeping() : false;
			if(auto* rigid = actors[i]->is<PxRigidDynamic>())
			{
				const auto& transform = rigid->getGlobalPose();
				TransformData data;
				data.position = {transform.p.x,transform.p.y,transform.p.z};
				float arr[3];
				quaternion2Euler(transform.q,arr);
				data.rotation = {arr[0],arr[1],arr[2]};
				data.rotation *= RAD_TO_DEG;
				rigid->userData = &data;
			}
			//for(PxU32 j = 0; j < nbShapes; j++)
			//{
			//	shapePose(PxShapeExt::getGlobalPose(*shapes[j],*actors[i]));
			//	const PxGeometry& geom = shapes[j]->getGeometry();
			//	shapePose.getPosition();

			//	/*const bool isTrigger = cb ? cb->isTrigger(shapes[j]) : shapes[j]->getFlags() & PxShapeFlag::eTRIGGER_SHAPE;
			//	if(isTrigger)
			//		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);*/
			//		// render object
			//	glPushMatrix();
			//	glMultMatrixf(&shapePose.column0.x);
			//}
		}


		//Snippets::renderActors(&actors[0],static_cast<PxU32>(actors.size()),true);
	}

	//Snippets::finishRender();
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

PxScene* Shipyard_PhysX::GetScene()
{
	return gScene;
}

bool Shipyard_PhysX::AssignActor(TransformData* dataPtr)
{
	if(dataPtr)
	{
		createDynamic(PxTransform(PxVec3(dataPtr->position.x,dataPtr->position.y,dataPtr->position.z)),PxSphereGeometry(10),PxVec3(0,0,0),dataPtr);
	}
	else
	{
		createDynamic(PxTransform(PxVec3(0,0,0)),PxSphereGeometry(10),PxVec3(0,-0,-0),dataPtr);
	}
	return true;
}

physx::PxActor* Shipyard_PhysX::GetActor(int aActorID)
{
	aActorID;
	return nullptr;
}

//void Shipyard_UserErrorCallback::reportError(physx::PxErrorCode::Enum code,const char* message,const char* file,int line)
//{
//	std::cout << "PhysX Error : (" << (int)code << ") " << message << " in " << file << " at line " << line << std::endl;
//	// error processing implementation 
//}