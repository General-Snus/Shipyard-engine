#pragma once
class ParticleRenderer
{
	friend class GraphicsEngine;
public:
	void Init();
	void Execute();
private:

	ComPtr<ID3DBlob> vertexShader;
	ComPtr<ID3DBlob> geometryShader;
	ComPtr<ID3DBlob> pixelShader;
};

