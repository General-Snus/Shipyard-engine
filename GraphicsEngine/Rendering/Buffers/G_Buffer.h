#pragma once

#include <ThirdParty/CU/CommonUtills/Matrix4x4.hpp>
#include "Rendering/Buffers/ConstantBuffer.h" 
#include <AssetManager/Objects/BaseAssets/MaterialAsset.h>

struct G_BufferData
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

	//Custom shader 
	ComPtr<ID3D11VertexShader> myVertexShader;
	ComPtr<ID3D11PixelShader> myPixelShader;
	std::shared_ptr<Shader> defaultVS;
	std::shared_ptr<Shader> defaultPS;

	int mask = 0;
	/* Can i send Gbuffer as a light?
	std::shared_ptr<Texture> albedoTexture;
	std::shared_ptr<Texture> normalTexture;
	std::shared_ptr<Texture> materialTexture;
	std::shared_ptr<Texture> effectTexture;
	
	
	*/

	void SetMaterialBuffer(std::vector<std::shared_ptr<TextureHolder>> textures);
};

typedef ConstantBuffer<G_BufferData> G_Buffer;