#pragma once

class CommandList;
class Passes
{
public:
	static void WriteShadows(std::shared_ptr<CommandList>& commandList, GameObjectManager& scene);
	static LightBuffer CreateLightBuffer(GameObjectManager& scene);
};