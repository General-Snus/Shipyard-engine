// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef GRAPHICSENGINE_PCH
#define GRAPHICSENGINE_PCH

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define NOMINMAX
#define CU CommonUtilities

// add headers that you want to pre-compile here
#include <Windows.h>
#include <d3d11.h>
#include "InterOp/Helpers.h"
#include "InterOp/RHI.h"
#include "Logging/Logging.h"
#include "GraphicsEngine.h"


#include <GraphicsEngine/Rendering/Buffers/FrameBuffer.h>
#include <GraphicsEngine/Rendering/Buffers/ObjectBuffer.h>
#include <GraphicsEngine/Rendering/Buffers/LightBuffer.h>
#include <GraphicsEngine/Rendering/Buffers/LineBuffer.h>
#include <GraphicsEngine/Rendering/Buffers/G_buffer.h>

#include <GraphicsEngine/GraphicCommands/Commands/Headers/GfxCmd_DebugLayer.h>
#include <GraphicsEngine/GraphicCommands/Commands/Headers/GfxCmd_DrawDebugPrimitive.h>
#include <GraphicsEngine/GraphicCommands/Commands/Headers/GfxCmd_RenderMesh.h>
#include <GraphicsEngine/GraphicCommands/Commands/Headers/GfxCmd_RenderMeshShadow.h>
#include <GraphicsEngine/GraphicCommands/Commands/Headers/GfxCmd_RenderSkeletalMesh.h>
#include <GraphicsEngine/GraphicCommands/Commands/Headers/GfxCmd_RenderSkeletalMeshShadow.h>
#include <GraphicsEngine/GraphicCommands/Commands/Headers/GfxCmd_RenderSkybox.h>
#include <GraphicsEngine/GraphicCommands/Commands/Headers/GfxCmd_SetFrameBuffer.h>
#include <GraphicsEngine/GraphicCommands/Commands/Headers/GfxCmd_SetLightBuffer.h>
#include <GraphicsEngine/GraphicCommands/Commands/Headers/GfxCmd_SetRenderTarget.h>



static inline Logger GELogger;
#endif //GRAPHICSENGINE_PCH
