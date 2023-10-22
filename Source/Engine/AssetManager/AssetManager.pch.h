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

#include <Tools/Utilities/LinearAlgebra/Matrix4x4.hpp>
#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>
#include <Tools/Utilities/Game/Timer.h>
#include <Tools/Utilities/Math.hpp>
#include "Tools/Logging/Logging.h"

#include "Objects/BaseAssets/BaseAsset.h"
#include "Objects/BaseAssets/Animations.h"
#include "Objects/BaseAssets/MaterialAsset.h"
#include "Objects/BaseAssets/MeshAsset.h"
#include "Objects/BaseAssets/TextureAsset.h" 

#include "Objects/Components/Component.h" 
#include "GameObjectManager.h"
#include "Objects/GameObjects/GameObject.h"
#include "Objects/Components/ComponentManager.h"
#include "Objects/Components/ComponentDerivatives/MeshRenderer.h"
#include "Objects/Components/ComponentDerivatives/Transform.h"
#include "Objects/Components/ComponentDerivatives/Animator.h"
#include "Objects/Components/ComponentDerivatives/Skybox.h"
#include "Objects/Components/ComponentDerivatives/CameraComponent.h"
#include "Objects/Components/ComponentDerivatives/LightComponent.h"
#include "Objects/Components/ComponentDerivatives/ParticleSystem.h"
#include "Objects/Components/ComponentDerivatives/DEBUGCOMPONENTS/BackgroundColor.h"
#include "Objects/Components/ComponentDerivatives/DEBUGCOMPONENTS/FrameStatistics.h"
#include "Objects/Components/ComponentDerivatives/DEBUGCOMPONENTS/RenderMode.h"

#include <Engine/GraphicsEngine/InterOp/RHI.h>
#include <Engine/GraphicsEngine/GraphicsEngine.h>
#include <Tools/FBX/TGAFbx.h>
#include <Engine/GraphicsEngine/Rendering/Vertex.h> 
#include <Engine/GraphicsEngine/GraphicCommands/GraphicCommands.h>
#include <Engine/GraphicsEngine/DebugDrawer/DebugDrawer.h>
#include "AssetManager.h"

typedef CU::Vector3<float> Vector3f;
typedef CU::Matrix4x4<float> Matrix;

static inline Logger AMLogger;
#endif //ASSETMANAGER_PCH_H 