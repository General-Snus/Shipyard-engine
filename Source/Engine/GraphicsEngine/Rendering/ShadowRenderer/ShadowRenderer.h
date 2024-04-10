#pragma once
class ShadowRenderer
{
	friend class GraphicsEngine;
public:
	void Init();
	void Execute();
	void ResetShadowList();
private:

	ComPtr<ID3DBlob> myVertexShader;
	//GraphicsCommandList ShadowCommandList;
};

