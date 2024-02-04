#pragma once 
#include <Engine/GraphicsEngine/Rendering/Buffers/ConstantBuffer.h>
#include <Engine/GraphicsEngine/Rendering/Texture.h>
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.hpp> 
#include <vector>

enum class eGbufferTex
{
	Albedo = 0,
	Normal,
	Material,
	Effect,
	VertexNormals,
	Position,
	Depth
};

struct G_Buffer
{
	// Set default values from graphicsEngine
	void Init();

	Vector4f position;


	std::shared_ptr<Texture> albedoTexture;
	std::shared_ptr<Texture> normalTexture;
	std::shared_ptr<Texture> materialTexture;
	std::shared_ptr<Texture> effectTexture;
	std::shared_ptr<Texture> vertexNormalsTexture;
	std::shared_ptr<Texture> positionTexture;
	std::shared_ptr<Texture> depthTexture;
	std::shared_ptr<Texture> defaultCubeMap;

	std::vector<Texture*> vectorOfTextures;

	//Custom shader 
	ComPtr<ID3DBlob> myVertexShader;
	ComPtr<ID3DBlob> myPixelShader;
	ComPtr<ID3DBlob> myScreenSpaceShader;
	ComPtr<ID3DBlob> myEnviromentPixelShader;
	ComPtr<ID3DBlob> myDebugPixelShader;
	ComPtr<ID3DBlob> myPointPixelShader;
	ComPtr<ID3DBlob> mySpotShader;

	int mask = 0;
	/* Can i send Gbuffer as a light?
	std::shared_ptr<Texture> albedoTexture;
	std::shared_ptr<Texture> normalTexture;
	std::shared_ptr<Texture> materialTexture;
	std::shared_ptr<Texture> effectTexture;
	*/
	void SetWriteTargetToBuffer();
	void SetResourceTexture();
	void UnsetResources();
	void UseGBufferShader();
	void UseEnviromentShader();
	void UseDebugShader();
	void UsePointlightShader();
	void UseSpotlightShader();
	void ClearTargets();

	void SetTexture(eGbufferTex texture);
	void UnSetTexture(eGbufferTex texture);

};
