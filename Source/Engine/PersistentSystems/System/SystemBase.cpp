#include "Engine/PersistentSystems/PersistentSystems.pch.h"

#include "SystemBase.h"

SystemBase::SystemBase()
{
    SystemCollection::AddSystem(this);
}
