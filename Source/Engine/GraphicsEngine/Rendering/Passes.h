#pragma once

class CommandList;
class Passes
{
  public:
    friend class GraphicsEngine;
    static void WriteShadows(const GraphicsEngine &instance, std::shared_ptr<CommandList> &commandList,
                             GameObjectManager &scene);
    static LightBuffer CreateLightBuffer(GameObjectManager &scene);
};
