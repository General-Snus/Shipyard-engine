#include "PersistentSystems.pch.h"

#include "../PhysXInterpeter.h"
#include <Editor/Editor/Defines.h>
#include <Engine/AssetManager/Objects/BaseAssets/MeshAsset.h>
#include <External/PhysX/pvd/PxPvdTransport.h>
#include <Tools/Utilities/Math.hpp>
#include <Tools/Utilities/Game/Timer.h>

#include "External/PhysX/PxPhysics.h"
#include "External/PhysX/PxPhysicsAPI.h"
#include "External/PhysX/omnipvd/PxOmniPvd.h"
#include "External/PhysX/omnipvd/PxOmniPvd.h"
#include "External/PhysX/cooking/PxCooking.h"
#include "Engine/AssetManager/AssetManager.h"
#include "Engine/GraphicsEngine/DebugDrawer/DebugDrawer.h"
#include "External/PhysX/geometry/PxTriangleMeshGeometry.h"

using namespace physx;


#pragma region hide this shit
void threeaxisrot(float r11,float r12,float r21,float r31,float r32,float res[]) {
	res[0] = atan2(r31,r32);
	res[1] = asin(r21);
	res[2] = atan2(r11,r12);
}

void quaternion2Euler(const PxQuat& q,float res[]) {
	threeaxisrot(-2 * (q.y * q.z - q.w * q.x),q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z,
		2 * (q.x * q.z + q.w * q.y),-2 * (q.x * q.y - q.w * q.z),
		q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z,res);
}
#pragma endregion

int Shipyard_PhysX::InitializePhysx() {
	OPTICK_EVENT();

	gAllocator = new PxDefaultAllocator();
	gErrorCallback = new PxDefaultErrorCallback();

	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION,*gAllocator,*gErrorCallback);

#if usingLegacyDebugger 
	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1",5425,100);
	gPvd->connect(*transport,PxPvdInstrumentationFlag::eALL);
#endif

	gOmniPvd = PxCreateOmniPvd(*gFoundation);
	if(gOmniPvd) {
		//Todo
	}

#if usingLegacyDebugger
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION,*gFoundation,PxTolerancesScale(),true,gPvd,gOmniPvd);
#else
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION,*gFoundation,PxTolerancesScale(),true,0,gOmniPvd);
#endif

	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f,-9.82f,0.0f);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;

	gScene = gPhysics->createScene(sceneDesc);
	gScene->setVisualizationParameter(PxVisualizationParameter::eSCALE,1.0f);
	gScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES,true);

	physx::PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if(pvdClient) {
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS,true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS,true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES,true);
	}



	gMaterial = gPhysics->createMaterial(0.5f,0.5f,0.6f);

	gScene->simulate(.1f);

	gScene->fetchResults(true);
	return 0;
}

void Shipyard_PhysX::StartRead() const {
#if PHYSX
	OPTICK_EVENT();
	gScene->fetchResults(true);
#endif
}

void Shipyard_PhysX::EndRead(float deltaTime) {
	deltaTime;
#if PHYSX
	OPTICK_EVENT();
	gScene->simulate(deltaTime);
#endif
}

void Shipyard_PhysX::Render() {

	if(!showLines) {
		if(!handles.empty()) {
			for(auto& i : handles) {
				RENDERER.debugDrawer.RemoveDebugPrimitive(i);
			}
			handles.clear();
		}
		return;
	}
	if(handles.empty()) {
		const PxRenderBuffer& rb = gScene->getRenderBuffer();
		for(PxU32 i = 0; i < rb.getNbLines(); i++) {
			const PxDebugLine& line = rb.getLines()[i];
			const Vector3f     Vpos1 = {line.pos0.x, line.pos0.y, line.pos0.z};
			const Vector3f     Vpos2 = {line.pos1.x, line.pos1.y, line.pos1.z};

			handles.emplace_back(RENDERER.debugDrawer.AddDebugLine(Vpos1,Vpos2,Vector3f(0,1,0)));
		}
	}
}

void Shipyard_PhysX::ShutdownPhysx() {
	PX_RELEASE(gScene);
	PX_RELEASE(gDispatcher);
	PX_RELEASE(gPhysics);
	if(gPvd) {
		PxPvdTransport* transport = gPvd->getTransport();
		gPvd->release();
		gPvd = nullptr;
		PX_RELEASE(transport);
	}
	PX_RELEASE(gFoundation);
}

void Shipyard_PhysX::resetScene() {
	gScene->fetchResults(true);
	PX_RELEASE(gScene);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f,-9.82f,0.0f);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;

	gScene = gPhysics->createScene(sceneDesc);
	gScene->setVisualizationParameter(PxVisualizationParameter::eSCALE,1.0f);
	gScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES,true);
}

void Shipyard_PhysX::ShowDrawLines(bool arg) { showLines = arg; }

PxPhysics* Shipyard_PhysX::GetPhysicsWorld() {
	return gPhysics;
}

PxScene* Shipyard_PhysX::GetScene() {
	return gScene;
}

// Walter white component
// template<>
// physx::PxTriangleMesh* Shipyard_PhysX::CookMesh<physx::PxTriangleMesh>(std::shared_ptr<Mesh> myToBeCookedMesh)
//{
//	if (myToBeCookedMesh->isLoadedComplete == false)
//	{
//		if (myToBeCookedMesh->isBeingLoaded == false)
//		{
//			EngineResources.ForceLoadAsset<Mesh>(myToBeCookedMesh->GetAssetPath(),myToBeCookedMesh);
//		}
//		while (myToBeCookedMesh->isLoadedComplete == false)
//		{
//		}
//	}
//
//	Element& element = myToBeCookedMesh->Elements[0];
//	const int vertCount = element.VertexBuffer.GetVertexCount();
//	const int indexCount = element.IndexResource.GetIndexCount();
//
//	PxVec3* convexVerts = new PxVec3[vertCount];
//	for (int i = 0; i < vertCount; i++)
//	{
//		Vector3f position = {
// element.Vertices[i].myPosition.x,element.Vertices[i].myPosition.y,element.Vertices[i].myPosition.z };
// convexVerts[i] = PxVec3(position.x,position.y,position.z);
//	}
//
//	PxU32* convexTris = new PxU32[indexCount];
//	for (int i = 0; i < indexCount; i++)
//	{
//		convexTris[i] = element.Indicies[i];
//	}
//
//	PxTriangleMeshDesc meshDesc;
//	meshDesc.points.count = vertCount;
//	meshDesc.points.stride = sizeof(PxVec3);
//	meshDesc.points.data = convexVerts;
//
//	meshDesc.triangles.count = indexCount / 3;
//	meshDesc.triangles.stride = 3 * sizeof(PxU32);
//	meshDesc.triangles.data = convexTris;
//
//	//convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX | PxConvexFlag::eDISABLE_MESH_VALIDATION |
// PxConvexFlag::eFAST_INERTIA_COMPUTATION; 	PxDefaultMemoryOutputStream buf; 	physx::PxTriangleMesh* tst;
//	physx::PxTriangleMeshCookingResult::Enum rst;
//	if (!PxCookTriangleMesh(PxTolerancesScale(),meshDesc,buf,&rst))
//	{
//		return NULL;
//	}
//	PxDefaultMemoryInputData* inpStream = new PxDefaultMemoryInputData(buf.getData(),buf.getSize());
//	tst = gPhysics->createTriangleMesh(*inpStream);
//	return tst;
// }

// void Shipyard_UserErrorCallback::reportError(physx::PxErrorCode::Enum code,const char* message,const char* file,int
// line)
//{
//	std::cout << "PhysX Error : (" << (int)code << ") " << message << " in " << file << " at line " << line <<
// std::endl;
//	// error processing implementation
// }
PxMaterial* Shipyard_PhysX::GetDefaultMaterial() {
	return gMaterial;
}

physx::PxShape* Shipyard_PhysX::CreateShape(PxRigidActor* actor,const PxGeometry* geometry,const PxMaterial* material,uint8_t shapeFlags) {

	PxShapeFlags pxShapeFlags = (PxShapeFlags)shapeFlags;
	PxShape* shape = gPhysics->createShape(*geometry,material != nullptr ? *material : *gMaterial,false,pxShapeFlags);
	actor->attachShape(*shape);
	shape->release();
	return shape;

	//return PxRigidActorExt::createExclusiveShape(
	//	*data,PxBoxGeometry(aabbData.GetXSize() / 2,aabbData.GetYSize() / 2,aabbData.GetZSize() / 2),
	//	*Shipyard_PhysXInstance.GetDefaultMaterial());
}
