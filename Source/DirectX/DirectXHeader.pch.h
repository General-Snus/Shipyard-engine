// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef DIRECTXHEADER_PCH_H
#define DIRECTXHEADER_PCH_H

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

#define USE_NSIGHT_AFTERMATH 0
#include <wrl.h>
using namespace Microsoft::WRL;

// DirectX 12 specific headers.
#include "DirectX/directx/d3dx12.h"
#include <DirectX/XTK/DescriptorHeap.h>
#include <DirectX/XTK/GraphicsMemory.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <dxgi1_6.h>
#include <dxgiformat.h>
// #include <DirectXTex.h>
using namespace DirectX;

// Other
#include "Engine/GraphicsEngine/Shaders/Registers.h"
#include <DirectX/Shipyard/Enums.h>
#include <Tools/Optick/include/optick.h>
#include <Tools/Utilities/Error.hpp>

// STL Headers
#include <DirectX/Shipyard/Helpers.h>
#include <algorithm>
#include <array>
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
#include <thread>
#include <unordered_map>
#include <vector>

#endif // DIRECTXHEADER_PCH_H
