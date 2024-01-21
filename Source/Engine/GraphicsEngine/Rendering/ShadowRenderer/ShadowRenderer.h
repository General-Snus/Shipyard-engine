#pragma once
#include <Engine/GraphicsEngine/GraphicCommands/GraphicCommands.h>
class ShadowRenderer
{
	friend class GraphicsEngine;
public:
	void Init();
	void Execute();
	void ResetShadowList();
private:

	ComPtr<ID3D11VertexShader> myVertexShader;
	GraphicsCommandList ShadowCommandList;
};

