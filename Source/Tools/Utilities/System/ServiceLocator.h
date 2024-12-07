#pragma once
#include "Executable/Executable/Export.h"
#include <Tools/Optick/include/optick.h>
#include <Tools/Utilities/Math.hpp>
#include <cassert>
#include <map> 
#include <memory>
#include <stdexcept>
#include <typeindex>

// Plan B active, implement shared memory for this service locator

class SHIPYARD_API Singleton;
class ServiceLocator
{
	friend class Singleton;

  public:
	SHIPYARD_API static ServiceLocator &Instance();

	static void SyncInstances(ServiceLocator &locator);

	template <typename T> T &GetService()
	{
		OPTICK_EVENT();
		const auto it = services.find(std::type_index(typeid(T)));
		if (it == services.end())
		{
			return ProvideService<T>();
		}
		return *static_cast<T *>(services[std::type_index(typeid(T))].get());
	}

	template <typename T> T &ProvideService()
	{
		OPTICK_EVENT();
		services[std::type_index(typeid(T))] = std::make_unique<T>();
		return *static_cast<T *>(services[std::type_index(typeid(T))].get());
	}

  private:
	std::map<std::type_index, std::unique_ptr<Singleton>> services;

	static inline ServiceLocator *instance;
	static constexpr unsigned long sharedMemorySize = 1024 * 1024;
	static void *OpenOrCreateSharedMemory();

	static void *MapSharedMemory(void *hMapFile);
	static void UnmapSharedMemory(void *pBuf);
};
