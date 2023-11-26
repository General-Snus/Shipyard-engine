// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef ASSETMANAGER_PCH_H
#define ASSETMANAGER_PCH_H
 
// add headers that you want to pre-compile here
#include <iostream> 
#include <filesystem> 
#include <unordered_map>
#include <memory>
#include <vector>
#include <functional>

#include <assimp/Importer.hpp>      // C++ importer interface 
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <Tools/Utilities/LinearAlgebra/Matrix4x4.hpp>
#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>
#include <Tools/Utilities/Game/Timer.h>
#include <Tools/Utilities/Math.hpp>
#include "Tools/Logging/Logging.h"
#include <Tools/Optick/src/optick.h>


#include "Objects/AI/NavMesh/NavMesh.h"
#include "Objects/AI/AgentSystem/Controllers/Controller.h"
#include "Objects/AI/AgentSystem/AIEventManager.h"
#include "Objects/AI/AgentSystem/Controllers/DefaultControllers/EventController.h"
#include "Objects/AI/AgentSystem/Controllers/PlayerController.h"  
#include "Objects/AI/AgentSystem/Controllers/WanderController.h"  

#include "Objects/BaseAssets/BaseAsset.h"
#include "Objects/BaseAssets/Animations.h"
#include "Objects/BaseAssets/MaterialAsset.h"
#include "Objects/BaseAssets/MeshAsset.h"
#include "Objects/BaseAssets/TextureAsset.h" 
#include "Objects/BaseAssets/LightDataBase.h" 

#include "ComponentSystem/Component.h" 
#include "ComponentSystem/GameObjectManager.h"
#include "ComponentSystem/GameObject.h"
#include "ComponentSystem/ComponentManager.h"
#include "ComponentSystem/Components/MeshRenderer.h"
#include "ComponentSystem/Components/Transform.h"
#include "ComponentSystem/Components/Animator.h"
#include "ComponentSystem/Components/Skybox.h"
#include "ComponentSystem/Components/CameraComponent.h"
#include "ComponentSystem/Components/LightComponent.h"
#include "ComponentSystem/Components/ParticleSystem.h"
#include "ComponentSystem/Components/ActorSystem/cActor.h"
#include "ComponentSystem/Components/Physics/cPhysics_Kinematic.h"
#include "ComponentSystem/Components/Physics/cPhysics_Newtonian.h"
#include "ComponentSystem/Components/DEBUGCOMPONENTS/BackgroundColor.h"
#include "ComponentSystem/Components/DEBUGCOMPONENTS/FrameStatistics.h"
#include "ComponentSystem/Components/DEBUGCOMPONENTS/RenderMode.h"

#include <Engine/GraphicsEngine/InterOp/RHI.h>
#include <Engine/GraphicsEngine/GraphicsEngine.h>
#include <Tools/FBX/TGAFbx.h>
#include <Engine/GraphicsEngine/Rendering/Vertex.h> 
#include <Engine/GraphicsEngine/GraphicCommands/GraphicCommands.h>
#include <Engine/GraphicsEngine/DebugDrawer/DebugDrawer.h>
#include "AssetManager.h"
#include "ComponentSystem/UUID.h"

static inline Logger AMLogger;

#define UseTGAImporter 0
#endif //ASSETMANAGER_PCH_H 