// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef ASSETMANAGER_PCH_H
#define ASSETMANAGER_PCH_H

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
// Native shipyard types,, will be moved to global include later for dependent projects
#include "Engine/AssetManager/Reflection/Reflectable.h"
#include "Tools/ImGui/imgui.h"
#include "Tools/Logging/Logging.h"
#include <External/Optick/include/optick.h>
#include <External/nlohmann/json.hpp>
#include <Tools/Reflection/refl.hpp>
#include <Tools/Utilities/Color.h>
#include <Tools/Utilities/Game/Timer.h>
#include <Tools/Utilities/Input/Input.hpp>
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.h>
#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>
#include <Tools/Utilities/Math.hpp>

//Assetmanager includes, need solution cause pch is actually bad
#include "ComponentSystem\Component.h"
#include "ComponentSystem\GameObject.h"
#include "ComponentSystem\Components\Transform.h"


// STL Headers
#include <algorithm>
#include <array>
#include <assert.h>
#include <assimp/Importer.hpp>	// C++ importer interface
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h>		// Output data structure
#include <cassert>
#include <cctype>
#include <cmath>
#include <concepts> 
#include <exception>
#include <filesystem> 
#include <fstream>
#include <functional>
#include <iostream>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <unknwn.h>
#include <objidl.h>
#include <oaidl.h>
#include <memory>
#include <random>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <vector> 

#include "ComponentSystem/UUID.h"
#include <Editor/Editor/Defines.h>
#define UseTGAImporter 0
#endif // ASSETMANAGER_PCH_H
