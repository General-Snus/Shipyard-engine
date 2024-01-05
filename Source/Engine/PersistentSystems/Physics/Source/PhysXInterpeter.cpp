#include "PersistentSystems.pch.h"
#include "../PhysXInterpeter.h" 


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
