// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PERSISTENT_PCH_H
#define PERSISTENT_PCH_H

#define NOMINMAX
// add headers that you want to pre-compile here
#include <iostream>
#include <string> 
#include <WS2tcpip.h> 
#include <vector>
#include <string>
#include <thread>
#include <chrono>

#include <Tools/Logging/Logging.h>
#include <Tools/Reflection/refl.hpp>
#include "Scene.h"
#include "ArtificialInteligence/AICommands/AICommands.h"
#include "Engine/GraphicsEngine/Renderer.h"
#include "Physics/PhysXInterpeter.h"
#include "Physics/Raycast.h"
#endif // PERSISTENT_PCH_H
