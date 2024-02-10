// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef ASSETMANAGER_PCH_H
#define ASSETMANAGER_PCH_H

#define NOMINMAX
// add headers that you want to pre-compile here
#include <algorithm> 
#include <assimp/Importer.hpp>      // C++ importer interface 
#include <assimp/postprocess.h>     // Post processing flags
#include <assimp/scene.h>           // Output data structure
#include <cassert>
#include <cmath>  
#include <filesystem>  
#include <fstream>
#include <functional> 
#include <iostream>  
#include <memory>
#include <random>
#include <ranges>
#include <Tools/ThirdParty/nlohmann/json.hpp>
#include <typeinfo> 
#include <unordered_map>
#include <vector>

#include <Tools/Optick/src/optick.h> 
#include <Tools/Utilities/Game/Timer.h>
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.hpp>
#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>
#include <Tools/Utilities/Math.hpp>
#include "Tools/Logging/Logging.h"

#include "Objects/AI/AgentSystem/AIEventManager.h"
#include "Objects/AI/AgentSystem/Controllers/AIController.h"  
#include "Objects/AI/AgentSystem/Controllers/Controller.h" 
#include "Objects/AI/AgentSystem/Controllers/DefaultControllers/EventController.h"
#include "Objects/AI/AgentSystem/Controllers/PlayerController.h"  
#include "Objects/AI/AgentSystem/Controllers/SeparationController.h"
#include "Objects/AI/AgentSystem/Controllers/WanderController.h"  
#include "Objects/AI/NavMesh/NavMesh.h"

#include "Objects/AI/AgentSystem/AIPollingManager.h"  
#include "Objects/AI/AgentSystem/PollingStations/PollingStation.h"  
#include "Objects/AI/AgentSystem/PollingStations/Target_PollingStation.h"  
#include "Objects/AI/AgentSystem/StateMachine/StateMachineBase.h"
#include "Objects/AI/AgentSystem/SteeringBehaviour.h"  

#include "Objects/BaseAssets/Animations.h"
#include "Objects/BaseAssets/BaseAsset.h"
#include "Objects/BaseAssets/LightDataBase.h" 
#include "Objects/BaseAssets/MaterialAsset.h"
#include "Objects/BaseAssets/MeshAsset.h"
#include "Objects/BaseAssets/TextureAsset.h" 
#include "Objects\BaseAssets\ColliderAsset.h"

#include "ComponentSystem/Component.h" 
#include "ComponentSystem/ComponentManager.h"
#include "ComponentSystem/Components/ActorSystem/cActor.h"
#include "ComponentSystem/Components/ActorSystem/CombatComponent.h"
#include "ComponentSystem/Components/Animator.h"
#include "ComponentSystem/Components/CameraComponent.h"
#include "ComponentSystem/Components/Collider.h"
#include "ComponentSystem/Components/DEBUGCOMPONENTS/BackgroundColor.h"
#include "ComponentSystem/Components/DEBUGCOMPONENTS/FrameStatistics.h"
#include "ComponentSystem/Components/DEBUGCOMPONENTS/RenderMode.h"
#include "ComponentSystem/Components/LightComponent.h"
#include "ComponentSystem/Components/MeshRenderer.h"
#include "ComponentSystem/Components/ParticleSystem.h"
#include "ComponentSystem/Components/Physics/cPhysics_Kinematic.h"
#include "ComponentSystem/Components/Physics/cPhysics_Newtonian.h"
#include "ComponentSystem/Components/Physics/cPhysXDynamicBody.h"
#include "ComponentSystem/Components/Physics/cPhysXStaticBody.h"
#include "ComponentSystem/Components/Skybox.h"
#include "ComponentSystem/Components/TaskSpecific/ProjectileComponent.h"
#include "ComponentSystem/Components/Transform.h"
#include "ComponentSystem/GameObject.h"
#include "ComponentSystem/GameObjectManager.h"

#include <Engine/GraphicsEngine/DebugDrawer/DebugDrawer.h>
#include <Engine/GraphicsEngine/GraphicCommands/GraphicCommands.h>
#include <Engine/GraphicsEngine/GraphicsEngine.h>
#include <Engine/GraphicsEngine/InterOp/RHI.h>
#include <Engine/GraphicsEngine/Rendering/Vertex.h> 
#include <Tools/FBX/TGAFbx.h>
#include "AssetManager.h"
#include "ComponentSystem/UUID.h"

#include <Editor/Editor/Defines.h> 

#define UseTGAImporter 0
#endif //ASSETMANAGER_PCH_H 