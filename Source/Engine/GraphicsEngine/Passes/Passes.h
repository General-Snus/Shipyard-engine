#pragma once
#include "Engine/GraphicsEngine/Rendering/Buffers/LightBuffer.h"
#include <memory>

class CommandList;
class GraphicsEngine;
class GameObjectManager;
class Scene;
class Camera;

namespace Passes {
	void ShadowPass(const GraphicsEngine& instance,std::shared_ptr<CommandList>& commandList,GameObjectManager& scene);
	LightBuffer CreateLightBuffer(GameObjectManager& scene);
}; // namespace Passes

namespace GBuffer {
	void Initialize(const GraphicsEngine& instance);
	void Render(const GraphicsEngine& instance,std::shared_ptr<CommandList>& commandList,
		const GameObjectManager& scene);
}

namespace SSAO {
	void Initialize(void);
	void Render(const GraphicsEngine& instance,std::shared_ptr<CommandList>& commandList,const Camera& camera);

	extern bool Enable;
} // namespace SSAO
