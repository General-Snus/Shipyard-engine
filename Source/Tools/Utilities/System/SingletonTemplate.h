#pragma once
#include "Executable/Executable/Export.h"
#include "Tools/Utilities/System/ServiceLocator.h"

// We inherite from this to store it in the service locator
// we can also delete the copy here
class SHIPYARD_API Singleton
{
	friend class ServiceLocator;

  public:
	Singleton() = default;
	~Singleton() = default;
};
