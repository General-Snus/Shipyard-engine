// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef GRAPHICSENGINE_PCH
#define GRAPHICSENGINE_PCH
#define NOMINMAX 

// add headers that you want to pre-compile here  
#include <filesystem>
#include <map>
#include <memory>
#include <mutex>  
#include <queue> 
#include <stdexcept> 
#include <string> 
#include <Tools/ImGui/ImGui/imgui.h>
#include <Tools/Optick/include/optick.h>  
#include <Tools/Utilities/Game/Timer.h> 
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.h>
#include <Windows.h>  
#include "GraphicsEngine.h" 
#include "Rendering/Buffers/FrameBuffer.h"
#include "Rendering/Buffers/G_buffer.h"
#include "Rendering/Buffers/LightBuffer.h"
#include "Rendering/Buffers/LineBuffer.h"
#include "Rendering/Buffers/ObjectBuffer.h"
#include "Rendering/ParticleRenderer/ParticleVertex.h"
#include "Rendering/Vertex.h" 
#include "Shaders/Registers.h" 
#include "Tools/Logging/Logging.h"
#include "wrl/client.h" 
#endif //GRAPHICSENGINE_PCH
