#pragma once
#include "Engine/GraphicsEngine/Rendering/Buffers/LightBuffer.h"
#include <memory>

class CommandList;
class GraphicsEngine;
class GameObjectManager;

namespace Passes
{
void WriteShadows(const GraphicsEngine &instance, std::shared_ptr<CommandList> &commandList, GameObjectManager &scene);
LightBuffer CreateLightBuffer(GameObjectManager &scene);
}; // namespace Passes
