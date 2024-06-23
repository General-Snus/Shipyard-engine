#pragma once    

#include "DirectX/Shipyard/Gpu_fwd.h"
#include "DirectX/Shipyard/PSO.h"
#include "Engine/AssetManager/Enums.h"  
#include "Rendering/Buffers/FrameBuffer.h"
#include "Rendering/Buffers/G_Buffer.h"
#include "Rendering/Buffers/GraphicSettingsBuffer.h"
#include "Rendering/Buffers/LightBuffer.h"
#include "Rendering/Buffers/LineBuffer.h"
#include "Rendering/Buffers/ObjectBuffer.h" 
#include <Engine/AssetManager/ComponentSystem/GameObjectManager.h>

#define _DEBUGDRAW 

class Viewport;
using namespace Microsoft::WRL;
class Mesh;
class Material;
class ShipyardShader;

struct GraphicsSettings
{
	int Tonemaptype = 0;
	bool DebugRenderer_Active = false;
};

enum class eRenderTargets
{
	BackBuffer,
	DepthBuffer,
	SceneBuffer,
	halfSceneBuffer,
	quaterSceneBuffer1,
	quaterSceneBuffer2,
	IntermediateA,
	IntermediateB,
	SSAO,
	NoiseTexture,
	count
};

enum class eShader
{
	defaultVS,
	defaultPS,
	particleVS,
	particleGS,
	particlePS,
	screenSpaceQuad,
	luminancePass,
	linearGammaPass,
	copyShader,
	gaussShader,
	bloomShader,
	debugLinePS,
	debugLineVS,
	count
};

class cCamera;
class Texture;
class TextureHolder;
class PSOCache;

class GraphicsEngine
{
	friend class GraphicCommandBase;
	friend class ShadowRenderer;
	friend class ParticleSystem;
	friend class InstanceRenderer;
public:
	enum class eDepthStencilStates : unsigned int
	{
		DSS_ReadWrite,
		DSS_ReadOnly,
		DSS_COUNT,
	};
	FrameBuffer myFrameBuffer;
private:
	ObjectBuffer myObjectBuffer;
	LineBuffer myLineBuffer;
	G_Buffer myG_Buffer;
	GraphicSettingsBuffer myGraphicSettingsBuffer;


	SIZE myWindowSize{ 0,0 };
	HWND myWindowHandle{};
	cCamera* myCamera;
	Vector4f myBackgroundColor;

	std::shared_ptr<Texture> myBackBuffer;
	std::shared_ptr<Texture> myDepthBuffer;

	std::shared_ptr<Texture> SceneBuffer;
	std::shared_ptr<Texture> halfSceneBuffer;
	std::shared_ptr<Texture> quaterSceneBuffer1;
	std::shared_ptr<Texture> quaterSceneBuffer2;

	std::shared_ptr<Texture> IntermediateA;
	std::shared_ptr<Texture> IntermediateB;


	//SSAO
	std::shared_ptr<Texture> SSAOTexture;
	ComPtr<ID3DBlob> ScreenSpaceAmbienceOcclusion;
	Microsoft::WRL::ComPtr<ID3DBlob> EdgeBlur;

	//Defualtl
	ComPtr<ID3DBlob> myVertexShader;
	ComPtr<ID3DBlob> myPixelShader;
	std::shared_ptr<ShipyardShader> defaultVS;
	std::shared_ptr<ShipyardShader> defaultPS;

	//Particle
	ComPtr<ID3DBlob> particleVertexShader;
	ComPtr<ID3DBlob> particleGeometryShader;
	ComPtr<ID3DBlob> particlePixelShader;


	//Post-pro
	ComPtr<ID3DBlob> myScreenSpaceQuadShader;
	ComPtr<ID3DBlob> luminancePass;
	ComPtr<ID3DBlob> TonemapPass;
	ComPtr<ID3DBlob> copyShader;
	ComPtr<ID3DBlob> gaussShader;
	ComPtr<ID3DBlob> bloomShader;

	static inline std::shared_ptr<Texture> BRDLookUpTable;
	static inline std::shared_ptr<TextureHolder> NoiseTable;
	static inline std::shared_ptr<TextureHolder> defaultTexture;
	static inline std::shared_ptr<TextureHolder> defaultNormalTexture;
	static inline std::shared_ptr<TextureHolder> defaultMatTexture;
	static inline std::shared_ptr<TextureHolder> defaultEffectTexture;
	static inline std::shared_ptr<TextureHolder> defaultParticleTexture;
	static inline std::shared_ptr<TextureHolder> defaultCubeMap;
	static inline std::shared_ptr<Mesh> defaultMesh;
	static inline std::shared_ptr<Material> defaultMaterial; 
	GraphicsSettings myGraphicSettings;
	HeapHandle ViewPortHeapHandle;
	// We're a container singleton, no instancing this outside the class.
	GraphicsEngine() = default;


private:
	static bool SetupDebugDrawline();
	void SetupDefaultVariables();
	static void SetupBlendStates();
	void SetupParticleShaders();
	void UpdateSettings();
	void SetupSpace3();
	void SetupPostProcessing();


	void BeginFrame();
	static uint64_t RenderFrame(Viewport& renderViewPort,GameObjectManager& scene);
	static void EndFrame();

	static void PrepareBuffers(std::shared_ptr<CommandList> commandList,Viewport& renderViewPort,GameObjectManager& scene);
	static void DeferredRenderingPass(std::shared_ptr<CommandList> commandList,Viewport& renderViewPort,GameObjectManager& scene);
	static void EnvironmentLightPass(std::shared_ptr<CommandList> commandList);
	static void ToneMapperPass(std::shared_ptr<CommandList> commandList,Texture* target);
	static void ImGuiPass(std::shared_ptr<CommandList> commandList); 
	void RenderTextureTo(eRenderTargets from,eRenderTargets to) const;
public:
	inline static GraphicsEngine& Get()
	{
		static GraphicsEngine myInstance;
		return myInstance;
	}
	bool Initialize(HWND windowHandle,bool enableDeviceDebug);
	void Render(  std::vector<std::shared_ptr<Viewport>>  renderViewPorts);

	 

	void SetDepthState(eDepthStencilStates state)
	{
		state;
		//RHI::Context->OMSetDepthStencilState(myDepthStencilStates[(int)state].Get(),0);
	}

	Vector4f& GetBackgroundColor() { return myBackgroundColor; }



	[[nodiscard]] HWND FORCEINLINE GetWindowHandle() const { return myWindowHandle; }
	[[nodiscard]] SIZE FORCEINLINE GetWindowSize() const { return myWindowSize; }


	FORCEINLINE std::shared_ptr<ShipyardShader> GetDefaultVSShader() const { return defaultVS; }
	FORCEINLINE std::shared_ptr<ShipyardShader> GetDefaultPSShader() const { return defaultPS; }

	FORCEINLINE ComPtr<ID3DBlob> GetQuadShader() const { return myScreenSpaceQuadShader; }
	FORCEINLINE ComPtr<ID3DBlob> GetLuminanceShader() const { return luminancePass; }
	FORCEINLINE ComPtr<ID3DBlob> GetSSAOShader() const { return ScreenSpaceAmbienceOcclusion; }
	FORCEINLINE ComPtr<ID3DBlob> GetEdgeBlurShader() const { return EdgeBlur; }
	FORCEINLINE ComPtr<ID3DBlob> GetTonemap() const { return TonemapPass; }
	FORCEINLINE ComPtr<ID3DBlob> GetCopyShader() const { return copyShader; }
	FORCEINLINE ComPtr<ID3DBlob> GetGaussShader() const { return gaussShader; }
	FORCEINLINE ComPtr<ID3DBlob> GetBloomShader() const { return bloomShader; }
	FORCEINLINE ComPtr<ID3DBlob> GetParticleVSShader() const { return particleVertexShader; }
	FORCEINLINE ComPtr<ID3DBlob> GetParticleGSShader() const { return particleGeometryShader; }
	FORCEINLINE ComPtr<ID3DBlob> GetParticlePSShader() const { return particlePixelShader; }  

	FORCEINLINE std::shared_ptr<Texture> GetTargetTextures(eRenderTargets type) const;

	FORCEINLINE GraphicsSettings& GetSettings() { return myGraphicSettings; }
	//FORCEINLINE std::shared_ptr<ShipyardShader> GetDebugLineVS() const { return debugLineVS; }
	//FORCEINLINE std::shared_ptr<ShipyardShader> GetDebugLinePS() const { return debugLinePS; }

	//FORCEINLINE ComPtr<ID3D11BlendState> GetAlphaBlendState() const { return AlphaBlendState; }
	//FORCEINLINE ComPtr<ID3D11BlendState> GetAdditiveBlendState() const { return AdditiveBlendState; }

	FORCEINLINE std::shared_ptr<Material> GetDefaultMaterial() const { return defaultMaterial; }
	FORCEINLINE std::shared_ptr<TextureHolder> GetDefaultTexture(eTextureType type) const
	{
		switch (type)
		{
		case eTextureType::ColorMap:
			return defaultTexture;

		case eTextureType::NormalMap:
			return defaultNormalTexture;

		case eTextureType::MaterialMap:
			return defaultMatTexture;

		case eTextureType::EffectMap:
			return defaultEffectTexture;

		case eTextureType::ParticleMap:
			return defaultParticleTexture;

		case eTextureType::CubeMap:
			return defaultCubeMap;

		default:
			return defaultTexture;
		}
	}
};