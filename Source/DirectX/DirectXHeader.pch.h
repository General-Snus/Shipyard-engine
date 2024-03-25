#pragma once 
#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>

#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif 

#define  USE_NSIGHT_AFTERMATH 0 
#include <wrl.h>
using namespace Microsoft::WRL;

// DirectX 12 specific headers.
#include <dxgi1_6.h> 
#include "DirectX/directx/d3dx12.h" 

#include <d3dcompiler.h> 
#include <DirectXMath.h> 
#include <DirectX/XTK/DescriptorHeap.h>
#include <DirectX/XTK/GraphicsMemory.h>   
//#include <DirectXTex.h>
using namespace DirectX;

// STL Headers
#include <array> 
#include <algorithm>
#include <atomic>
#include <cassert> 
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <filesystem>
#include <map> 
#include <memory> 
#include <mutex>  
#include <new>
#include <stdexcept>
#include <string> 
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>   
#include <DirectX/Shipyard/Helpers.h>
