// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef ASSETMANAGER_PCH_H
#define ASSETMANAGER_PCH_H

#define CU   CommonUtilities  
// add headers that you want to pre-compile here
#include <iostream> 
#include <filesystem> 
#include <unordered_map>
#include <memory>

#include <ThirdParty/CU/CommonUtills/Matrix4x4.hpp>
#include <ThirdParty/CU/CommonUtills/Vectors.hpp>
#include <ThirdParty/CU/Timer.h>

#include "Objects/Components/Component.h"

#include "Objects/BaseAssets/BaseAsset.h"
#include "Objects/BaseAssets/Animations.h"
#include "Objects/BaseAssets/MaterialAsset.h"
#include "Objects/BaseAssets/MeshAsset.h"
#include "Objects/BaseAssets/TextureAsset.h"




#include <TGAFbx.h> 
#include <GraphicsEngine/Rendering/Vertex.h>
#include <GraphicsEngine/GraphicsEngine.h>  
#include <GraphicsEngine/Commands/GraphicCommands.h>
#include <GraphicsEngine/DebugDrawer/DebugDrawer.h>
#include "AssetManager.h"
#include <ThirdParty/CU/Math.hpp>
typedef CU::Vector3<float> Vector3f;
typedef CU::Matrix4x4<float> Matrix;

#endif //ASSETMANAGER_PCH_H 