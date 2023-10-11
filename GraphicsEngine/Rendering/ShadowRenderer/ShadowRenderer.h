#pragma once
class ShadowRenderer
{
	friend class GraphicsEngine;
public:
	void Init();
	void Execute();
	void ResetShadowList();
private:

	ComPtr<ID3D11VertexShader> myVertexShader;
	ComPtr<ID3D11GeometryShader> myGeoShader;
	ComPtr<ID3D11PixelShader> myPixelShader;
	GraphicsCommandList ShadowCommandList;
};

