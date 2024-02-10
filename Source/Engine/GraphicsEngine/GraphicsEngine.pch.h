// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef GRAPHICSENGINE_PCH
#define GRAPHICSENGINE_PCH
#define NOMINMAX 

// add headers that you want to pre-compile here 
#include <DirectX/directx/d3d12.h>  
#include <DirectX/directx/d3dx12.h>
#include <DirectX/XTK/BufferHelpers.h>
#include <DirectX/XTK/CommonStates.h>
#include <DirectX/XTK/DirectXHelpers.h>
#include <DirectX/XTK/GraphicsMemory.h>

#include <Windows.h>
#include "GraphicCommands/Commands/Headers/GfxCmd_Bloom.h"
#include "GraphicCommands/Commands/Headers/GfxCmd_DebugLayer.h" 
#include "GraphicCommands/Commands/Headers/GfxCmd_DrawDebugPrimitive.h"
#include "GraphicCommands/Commands/Headers/GfxCmd_GaussianBlur.h" 
#include "GraphicCommands/Commands/Headers/GfxCmd_LuminancePass.h"
#include "GraphicCommands/Commands/Headers/GfxCmd_RenderMesh.h"
#include "GraphicCommands/Commands/Headers/GfxCmd_RenderMeshShadow.h"
#include "GraphicCommands/Commands/Headers/GfxCmd_RenderSkeletalMesh.h"
#include "GraphicCommands/Commands/Headers/GfxCmd_RenderSkeletalMeshShadow.h"
#include "GraphicCommands/Commands/Headers/GfxCmd_RenderSkybox.h"
#include "GraphicCommands/Commands/Headers/GfxCmd_SetDepthState.h"
#include "GraphicCommands/Commands/Headers/GfxCmd_SetFrameBuffer.h" 
#include "GraphicCommands/Commands/Headers/GfxCmd_SetLightBuffer.h" 
#include "GraphicCommands/Commands/Headers/GfxCmd_SetRenderTarget.h" 
#include "GraphicCommands/Commands/Headers/GfxCmd_SSAO.h" 
#include "GraphicCommands/Commands/Headers/GfxCmd_ToneMapPass.h"
#include "GraphicCommands/GraphicCommands.h"
#include "GraphicsEngine.h" 
#include "InterOp/Helpers.h" 
#include "Objects/Shader.h" 
#include "Rendering/Buffers/FrameBuffer.h"
#include "Rendering/Buffers/G_buffer.h"
#include "Rendering/Buffers/LightBuffer.h"
#include "Rendering/Buffers/LineBuffer.h"
#include "Rendering/Buffers/ObjectBuffer.h"
#include "Rendering/ParticleRenderer/ParticleVertex.h" 
#include "Rendering/Texture.h" 
#include "Rendering/Vertex.h" 
#include "Shaders/Registers.h" 
#include "Tools/Logging/Logging.h"
#include "wrl/client.h"
#endif //GRAPHICSENGINE_PCH
