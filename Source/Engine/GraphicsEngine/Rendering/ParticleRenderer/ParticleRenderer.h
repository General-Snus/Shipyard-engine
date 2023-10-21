#pragma once
class ParticleRenderer
{
	friend class GraphicsEngine;
public:
	void Init();
	void Execute(); 
private:

	ComPtr<ID3D11VertexShader> vertexShader;
	ComPtr<ID3D11GeometryShader> geometryShader;
	ComPtr<ID3D11PixelShader> pixelShader;
};

