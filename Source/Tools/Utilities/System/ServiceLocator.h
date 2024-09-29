#pragma once
#include "Executable/Executable/Export.h"
#include <Tools/Utilities/Math.hpp>
#include <Windows.h>
#include <cassert>
#include <memory>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>

// Plan B active, implement shared memory for this service locator

class SHIPYARD_API Singleton;
class ServiceLocator
{
    friend class Singleton;

  public:
    SHIPYARD_API static ServiceLocator &Instance()
    {
        if (!instance)
        {
            HANDLE hMapFile = OpenOrCreateSharedMemory();
            void *pBuf = MapSharedMemory(hMapFile);
            instance = new (pBuf) ServiceLocator();
        }

        return *instance;
    }

    static void SyncInstances(ServiceLocator &locator)
    {
        instance = &locator;
    }

    template <typename T> T &GetService()
    {
        auto it = services.find(std::type_index(typeid(T)));
        if (it == services.end())
        {
            auto service = std::make_unique<T>();
            services[std::type_index(typeid(T))] = std::move(service);
        }
        return *static_cast<T *>(services[std::type_index(typeid(T))].get());
    }

    template <typename T> T &ProvideService()
    {
        services[std::type_index(typeid(T))] = std::make_unique<T>();
        return *static_cast<T *>(services[std::type_index(typeid(T))].get());
    }

  private:
    std::unordered_map<std::type_index, std::unique_ptr<Singleton>> services;

    static inline ServiceLocator *instance;
    static constexpr DWORD sharedMemorySize = 1024 * 1024;
    static HANDLE OpenOrCreateSharedMemory()
    {
        HANDLE hMapFile = CreateFileMappingW(INVALID_HANDLE_VALUE, // Use the paging file
                                             NULL,                 // Default security
                                             PAGE_READWRITE,       // Read/write access
                                             0,                    // Max. object size (high-order DWORD)
                                             sharedMemorySize,     // Max. object size (low-order DWORD)
                                             L"ServiceLocatorSharedMemory");

        if (hMapFile == NULL || hMapFile == INVALID_HANDLE_VALUE)
        {
            throw std::runtime_error("Could not create or open shared memory.");
        }

        return hMapFile;
    }

    static void *MapSharedMemory(HANDLE hMapFile)
    {
        void *pBuf = MapViewOfFile(hMapFile,            // Handle to map object
                                   FILE_MAP_ALL_ACCESS, // Read/write permission
                                   0, 0, sharedMemorySize);

        if (pBuf == NULL)
        {
            CloseHandle(hMapFile);
            auto reson = GetLastError();
            throw std::runtime_error("Could not create map shared memory." + reson);
        }

        return pBuf;
    }
    static void UnmapSharedMemory(void *pBuf)
    {
        UnmapViewOfFile(pBuf);
    }
};
