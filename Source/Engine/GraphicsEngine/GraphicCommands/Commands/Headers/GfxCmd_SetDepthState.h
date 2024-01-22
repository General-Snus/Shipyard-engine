#pragma once
#include <Engine/GraphicsEngine/GraphicCommands/GraphicCommands.h>


enum class GraphicsEngine::eDepthStencilStates : unsigned int;


class GfxCmd_SetDepthState : public GraphicCommandBase
{
public:
	GfxCmd_SetDepthState(GraphicsEngine::eDepthStencilStates inp);
	void Destroy() override {};
	void ExecuteAndDestroy() override;
private:
	GraphicsEngine::eDepthStencilStates m_DepthState;
};

