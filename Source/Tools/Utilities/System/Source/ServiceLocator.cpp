#include "../ServiceLocator.h" 
#include <Tools/Utilities/System/SingletonTemplate.h>
#include <Tools/Utilities/Math.hpp>
#include <Windows.h>
#include <string>

// Definition of the static member
ServiceLocator& ServiceLocator::Instance() {
	if(!instance) {
		const HANDLE hMapFile = OpenOrCreateSharedMemory();
		void* pBuf = MapSharedMemory(hMapFile);
		instance = new (pBuf) ServiceLocator();
	}

	return *instance;
}

void ServiceLocator::SyncInstances(ServiceLocator& locator) {
	instance = &locator;
}

void* ServiceLocator::OpenOrCreateSharedMemory() {
	
	;
	std::wstring name = L"ServiceLocatorSharedMemory_" + std::format(L"{}-{}",
		Math::RandomEngine::randomInRange<int>(INT_MIN,INT_MAX),
		Math::RandomEngine::randomInRange<int>(INT_MIN,INT_MAX)
	);

 	const HANDLE hMapFile = CreateFileMappingW(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,sharedMemorySize,
		name.c_str());

	if(hMapFile == NULL || hMapFile == INVALID_HANDLE_VALUE) {
		throw std::runtime_error("Could not create or open shared memory.");
	}

	return hMapFile;
}

void* ServiceLocator::MapSharedMemory(void* hMapFile) {
	void* pBuf = MapViewOfFile(hMapFile,			// Handle to map object
		FILE_MAP_ALL_ACCESS, // Read/write permission
		0,0,sharedMemorySize);

	if(pBuf == NULL) {
		CloseHandle(hMapFile);
		const auto reson = GetLastError();
		throw std::runtime_error("Could not create map shared memory." + reson);
	}

	return pBuf;
}

void ServiceLocator::UnmapSharedMemory(void* pBuf) {
	UnmapViewOfFile(pBuf);
}