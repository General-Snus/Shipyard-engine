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

#include "Objects/BaseAssets/BaseAsset.h"
#include "Objects/BaseAssets/Animations.h"
#include "Objects/BaseAssets/MaterialAsset.h"
#include "Objects/BaseAssets/MeshAsset.h"
#include "Objects/BaseAssets/TextureAsset.h" 


#include "Objects/Components/Component.h"
#include <AssetManager/GameObjectManager.h>
#include <AssetManager/Objects/GameObjects/GameObject.h> 
#include <AssetManager/Objects/Components/ComponentManager.h>
#include <AssetManager/Objects/Components/ComponentDerivatives/MeshRenderer.h>
#include <AssetManager/Objects/Components/ComponentDerivatives/Transform.h>
#include <AssetManager/Objects/Components/ComponentDerivatives/Animator.h>
#include <AssetManager/Objects/Components/ComponentDerivatives/Skybox.h>
#include <AssetManager/Objects/Components/ComponentDerivatives/CameraComponent.h>
#include <AssetManager/Objects/Components/ComponentDerivatives/LightComponent.h>
#include <AssetManager/Objects/Components/ComponentDerivatives/DEBUGCOMPONENTS/BackgroundColor.h>
#include <AssetManager/Objects/Components/ComponentDerivatives/DEBUGCOMPONENTS/FrameStatistics.h>
#include <AssetManager/Objects/Components/ComponentDerivatives/DEBUGCOMPONENTS/RenderMode.h>

#include <TGAFbx.h> 
#include <GraphicsEngine/Rendering/Vertex.h> 
#include <GraphicsEngine/GraphicCommands/GraphicCommands.h>
#include <GraphicsEngine/DebugDrawer/DebugDrawer.h>
#include "AssetManager.h"
#include <ThirdParty/CU/Math.hpp>

typedef CU::Vector3<float> Vector3f;
typedef CU::Matrix4x4<float> Matrix;

#endif //ASSETMANAGER_PCH_H 