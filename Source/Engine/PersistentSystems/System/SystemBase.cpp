#include "PersistentSystems.pch.h"

#include "SystemBase.h"

SystemBase::SystemBase()
{
    SystemCollection::AddSystem(this);
}
