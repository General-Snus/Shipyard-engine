#pragma once
#include "Tools/ImGui/MuninGraph/Types/TypeRegistry.h"
#include "Tools/Utilities/LinearAlgebra/Vector3.hpp"
#include <Engine/AssetManager/ComponentSystem/GameObject.h>

#pragma comment(linker, "/include:__MuninGraph_AutoRegTypes")

//~ Internal type to handle Owner variable on ScriptGraphs.
DECLARE_REGISTERED_TYPE(void *);
//~

DECLARE_REGISTERED_TYPE(float);
DECLARE_REGISTERED_TYPE(bool);
DECLARE_REGISTERED_TYPE(size_t);
DECLARE_REGISTERED_TYPE(int);
DECLARE_REGISTERED_TYPE(Vector3f);
DECLARE_REGISTERED_TYPE(GameObject);
DECLARE_REGISTERED_TYPE(SY::UUID); 
DECLARE_REGISTERED_TYPE(std::string);
