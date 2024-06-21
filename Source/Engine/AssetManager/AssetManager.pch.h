// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef ASSETMANAGER_PCH_H
#define ASSETMANAGER_PCH_H

#define NOMINMAX


//Native shipyard types,, will be moved to global include later for dependent projects
#include <Engine/AssetManager/ComponentSystem/GameObject.h>
#include <Engine/AssetManager/ComponentSystem/Component.h>
#include <Engine/AssetManager/ComponentSystem/MasterIncludeComponent.h>  
#include <Engine/AssetManager/Objects/BaseAssets/MasterIncludeAssets.h>
#include "Engine/PersistentSystems/Scene.h"

// add headers that you want to pre-compile here
#include <Tools/Reflection/refl.hpp>
#include <Tools/ThirdParty/nlohmann/json.hpp>
#include "Engine/AssetManager/Reflection/ReflectionTemplate.h" 
#include <Tools/Optick/include/optick.h> 
#include <Tools/Utilities/Game/Timer.h>
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.h>
#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>
#include <Tools/Utilities/Math.hpp>
#include "Tools/ImGui/ImGui/imgui.h"
#include "Tools/Logging/Logging.h"    
#include <Tools/Utilities/Input/Input.hpp>
#include <Tools/Utilities/Math.hpp>

// STL Headers 
#include <algorithm>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream> 
#include <algorithm> 
#include <assimp/Importer.hpp>      // C++ importer interface 
#include <assimp/postprocess.h>     // Post processing flags
#include <assimp/scene.h>           // Output data structure
#include <cassert>
#include <assert.h>
#include <cctype>
#include <cmath>  
#include <filesystem> 
#include <fstream>
#include <functional> 
#include <iostream>  
#include <memory>
#include <filesystem>
#include <functional>
#include <iostream> 
#include <typeinfo>
#include <unordered_map>
#include <vector> 
#include <random>
#include <ranges>
#include <string>
#include <typeinfo> 
#include <unordered_map>
#include <vector>
/*
#include <Engine/GraphicsEngine/DebugDrawer/DebugDrawer.h>
#include <Engine/GraphicsEngine/GraphicsEngine.h>
#include <Engine/GraphicsEngine/InterOp/RHI.h>
#include <Engine/GraphicsEngine/Rendering/Vertex.h>
*/
#include <Editor/Editor/Defines.h>  
#include "AssetManager.h"
#include "ComponentSystem/UUID.h" 
#define UseTGAImporter 0
#endif //ASSETMANAGER_PCH_H 