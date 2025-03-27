
#ifndef ENGINGE_PCH_H
#define ENGINGE_PCH_H

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

// Component Internal
#include "Engine/PersistentSystems/Scene.h"
#include <Engine/AssetManager/ComponentSystem/Component.h>
#include <Engine/AssetManager/ComponentSystem/GameObject.h>
#include <Engine/AssetManager/ComponentSystem/MasterIncludeComponent.h>
#include <Engine/AssetManager/ComponentSystem/UUID.h>
#include <Engine/AssetManager/Objects/BaseAssets/MasterIncludeAssets.h>

// Utilities and third party
#include "Engine/AssetManager/Reflection/ReflectionTemplate.h"
#include "Tools/ImGui/imgui.h"
#include "Tools/Logging/Logging.h"
#include <Tools/Optick/include/optick.h>
#include <Tools/Reflection/refl.hpp>
#include <Tools/ThirdParty/nlohmann/json.hpp>
#include <Tools/Utilities/DataStructures/Ranges.hpp>
#include <Tools/Utilities/Game/Timer.h>
#include <Tools/Utilities/Input/Input.hpp>
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.h>
#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>
#include <Tools/Utilities/Math.hpp>
#include <Tools/Utilities/Color.h>

// STL Headers and Assimp
#include <algorithm>
#include <assert.h>
#include <cassert>
#include <cctype>
#include <cmath>
#include <exception>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <random>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Asset and engine internal
#include <Editor/Editor/Defines.h>
#include <Engine/AssetManager/AssetManager.h>
#include <Engine/AssetManager/AssetManagerUtills.hpp>
#include <Engine/GraphicsEngine/Renderer.h>

// AI
#include "Engine/AssetManager/Objects/AI/AgentSystem/PollingStations/Target_PollingStation.h"
#include <Engine/AssetManager/ComponentSystem/Components/ActorSystem/CombatComponent.h>
#include <Engine/AssetManager/ComponentSystem/Components/Physics/cPhysics_Kinematic.h>
#include <Engine/AssetManager/ComponentSystem/Components/Transform.h>
#include <Engine/AssetManager/Objects/AI/AgentSystem/AIPollingManager.h>
#include <Engine/AssetManager/Objects/AI/AgentSystem/SteeringBehaviour.h>
#include <Engine/PersistentSystems/ArtificialInteligence/AICommands/AICommands.h>

#endif
