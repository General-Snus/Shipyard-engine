#include "GraphicsEngine.pch.h" 

GfxCmd_SetDepthState::GfxCmd_SetDepthState(GraphicsEngine::eDepthStencilStates inp) : m_DepthState(inp)
{
}

void GfxCmd_SetDepthState::ExecuteAndDestroy()
{
	GraphicsEngine::Get().SetDepthState(m_DepthState);
}
