#include "PersistentSystems.pch.h"
#include "../PhysXInterpeter.h" 
#include <PxPhysicsAPI.h>
#include <extensions/PxExtensionsAPI.h>
#include <extensions/PxDefaultSimulationFilterShader.h>
#include <extensions/PxDefaultCpuDispatcher.h>
#include <extensions/PxShapeExt.h>
#include <foundation/PxMat33.h> 
#include <extensions/PxSimpleFactory.h>

int Shipyard_PhysX::InitializePhysx()
{
    using namespace physx;

    mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback,gDefaultErrorCallback);

    if (mFoundation)
    {
        assert(false &&"PxCreateFoundation failed!"); 
    }
    PxTolerancesScale worldPhysicsScale; //#Planetarium Pay Attention here if imp planetary logic

    bool recordMemoryAllocations = true;
    mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation,
        worldPhysicsScale, recordMemoryAllocations, mPvd);

    if (!mPhysics)
    {
        assert(false && "PxCreatePhysics failed!");
    }


    if (!PxInitExtensions(*mPhysics, mPvd))
    {
        assert(false && "PxInitExtensions failed!");

    }



	return 0;
}

void Shipyard_PhysX::ShutdownPhysx()
{
	mPhysics->release();
	mPvd->release();
	mFoundation->release();
}


void Shipyard_UserErrorCallback::reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line)
{
    std::cout << "PhysX Error : (" << (int)code << ") " << message << " in " << file << " at line " << line << std::endl;
    // error processing implementation 
}