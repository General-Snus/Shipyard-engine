// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef DIRECTXHEADER_PCH_H
#define DIRECTXHEADER_PCH_H

#pragma warning(disable : 4201) // nonstandard extension used : nameless struct/union
#pragma warning(disable : 4238) // nonstandard extension used : class rvalue used as lvalue
#pragma warning(disable : 4239) // A non-const reference may only be bound to an lvalue; assignment operator takes a
								// reference to non-const
#pragma warning(disable : 4324) // structure was padded due to __declspec(align())

#include <winsdkver.h>
#define _WIN32_WINNT 0x0A00
#include <sdkddkver.h>

// Use the C++ standard templated min/max
#define NOMINMAX

// DirectX apps don't need GDI
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

// Include <mcx.h> if you need this
#define NOMCX

// Include <winsvc.h> if you need this
#define NOSERVICE

// WinHelp is deprecated
#define NOHELP

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <wrl/client.h>
#include <wrl/event.h>

#include "Helpers/d3dx12.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#ifdef _DEBUG
#include <dxgidebug.h>
#endif

#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cwctype>
#include <exception>
#include <memory>
#include <string>
#include <vector>

#include <functional>
#include <ppltasks.h>

#include <DescriptorHeap.h>
#include <DirectXHelpers.h>
#include <DirectXMath.h>
#include <DirectXTex.h>
#include <GraphicsMemory.h>
#include <ResourceUploadBatch.h>
#include <d3dcompiler.h>
#include <dxcapi.h>
#include <dxgiformat.h>

// Other
#include "Engine/GraphicsEngine/Shaders/Registers.h"
#include "Graphics/Enums.h"
#include <Tools/Optick/include/optick.h>
#include <Tools/Utilities/Error.hpp>

// STL Headers
#include "Graphics/Helpers.h"
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
