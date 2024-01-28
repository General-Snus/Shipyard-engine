#pragma once 
#include <Engine/AssetManager/Objects/BaseAssets/MeshAsset.h>
#include <Engine/AssetManager/Objects/BaseAssets/TextureAsset.h>
#include <Engine/GraphicsEngine/GraphicCommands/GraphicCommands.h>
#include <Engine/GraphicsEngine/Shaders/Registers.h> 
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.hpp> 
//#include "InterOp/RHI.h"
#include "InterOp/GPU.h"
#include "InterOp/PSO.h"

#include "Rendering/Buffers/ConstantBuffer.h" 
#include "Rendering/Buffers/FrameBuffer.h"
#include "Rendering/Buffers/G_Buffer.h"
#include "Rendering/Buffers/GraphicSettingsBuffer.h"
#include "Rendering/Buffers/LineBuffer.h"
#include "Rendering/Buffers/ObjectBuffer.h" 
#include "Rendering/ParticleRenderer/ParticleRenderer.h"
#include "Rendering/ShadowRenderer/ShadowRenderer.h"  
#define _DEBUGDRAW

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
private:

	std::unique_ptr<PSOCache> m_StateCache;


	FrameBuffer myFrameBuffer;
	ObjectBuffer myObjectBuffer;
	LineBuffer myLineBuffer;
	LightBuffer myLightBuffer;
	G_Buffer myG_Buffer;
	GraphicSettingsBuffer myGraphicSettingsBuffer;
	//Fill with render data for the deffered pass
	GraphicsCommandList DeferredCommandList;
	GraphicsCommandList OverlayCommandList;

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
	ComPtr<ID3D11PixelShader> ScreenSpaceAmbienceOcclusion;
	ComPtr<ID3D11PixelShader> EdgeBlur;

	//Defualtl
	ComPtr<ID3D11VertexShader> myVertexShader;
	ComPtr<ID3D11PixelShader> myPixelShader;
	std::shared_ptr<Shader> defaultVS;
	std::shared_ptr<Shader> defaultPS;

	//Particle
	ComPtr<ID3D11VertexShader> particleVertexShader;
	ComPtr<ID3D11GeometryShader> particleGeometryShader;
	ComPtr<ID3D11PixelShader> particlePixelShader;


	//Post-pro
	ComPtr<ID3D11VertexShader> myScreenSpaceQuadShader;
	ComPtr<ID3D11PixelShader> luminancePass;
	ComPtr<ID3D11PixelShader> TonemapPass;
	ComPtr<ID3D11PixelShader> copyShader;
	ComPtr<ID3D11PixelShader> gaussShader;
	ComPtr<ID3D11PixelShader> bloomShader;


	//Debug
	ComPtr<ID3D11Buffer> myLineVertexBuffer;
	ComPtr<ID3D11Buffer> myLineIndexBuffer;
	std::shared_ptr< Shader>  debugLineVS;
	std::shared_ptr< Shader> debugLinePS;

	std::shared_ptr<Texture> BRDLookUpTable;
	std::shared_ptr<TextureHolder> NoiseTable;
	std::shared_ptr<TextureHolder> defaultTexture;
	std::shared_ptr<TextureHolder> defaultNormalTexture;
	std::shared_ptr<TextureHolder> defaultMatTexture;
	std::shared_ptr<TextureHolder> defaultEffectTexture;
	std::shared_ptr<TextureHolder> defaultParticleTexture;
	std::shared_ptr<TextureHolder> defaultCubeMap;

	std::shared_ptr<Mesh> defaultMesh;
	std::shared_ptr<Material> defaultMaterial;

	ComPtr<ID3D11SamplerState> myDefaultSampleState;
	ComPtr<ID3D11SamplerState> myShadowSampleState;
	ComPtr<ID3D11SamplerState> myPointSampleState;
	ComPtr<ID3D11SamplerState> myBRDFSampleState;
	ComPtr<ID3D11SamplerState> myNormalDepthSampleState;



	std::array<ComPtr<ID3D11DepthStencilState>,(int)eDepthStencilStates::DSS_COUNT> myDepthStencilStates;

	ComPtr<ID3D11BlendState> AlphaBlendState;
	ComPtr<ID3D11BlendState> AdditiveBlendState;
	ShadowRenderer myShadowRenderer;
	ParticleRenderer myParticleRenderer;
	InstanceRenderer myInstanceRenderer;
	GraphicsSettings myGraphicSettings;

	// We're a container singleton, no instancing this outside the class.
	GraphicsEngine() = default;

public:
	inline static GraphicsEngine& Get()
	{
		static GraphicsEngine myInstance;
		return myInstance;
	}

	/**
	 * Initializes the Graphics Engine with the specified settings.
	 * @param windowHandle The window that will contain this Graphics Engine.
	 * @param enableDeviceDebug If DirectX should write debug output in the Output.
	 */
	bool Initialize(HWND windowHandle,bool enableDeviceDebug);

	bool SetupDebugDrawline();

	void SetupDefaultVariables();

	void SetupBlendStates();

	void SetupParticleShaders();

	void UpdateSettings();

	void SetupBRDF();

	void SetupPostProcessing();

	/**
	 * Prepares the next frame for rendering by resetting states and clearing all render targets.
	 */
	void BeginFrame();

	/**
	 * Finishes a rendered frame and swaps the screen buffers to display it on screen.
	 */
	void EndFrame();

	/**
	 * Renders the current scene to the BackBuffer.
	 */
	void RenderFrame(float aDeltaTime,double aTotalTime);

	void RenderTextureTo(eRenderTargets from,eRenderTargets to) const;


	void SetDepthState(eDepthStencilStates state)
	{
		state;
		//RHI::Context->OMSetDepthStencilState(myDepthStencilStates[(int)state].Get(),0);
	}

	Vector4f& GetBackgroundColor() { return myBackgroundColor; }


	template<typename CommandClass,typename ...Args>
	FORCEINLINE void ShadowCommands(Args... args)
	{
		myShadowRenderer.ShadowCommandList.AddCommand<CommandClass>(args...);
	}

	template<typename CommandClass,typename ...Args>
	FORCEINLINE void DeferredCommand(Args... args)
	{
		DeferredCommandList.AddCommand<CommandClass>(args ...);
	}

	template<typename CommandClass,typename ...Args>
	FORCEINLINE void OverlayCommands(Args... args)
	{
		OverlayCommandList.AddCommand<CommandClass>(args ...);
	}


	[[nodiscard]] HWND FORCEINLINE GetWindowHandle() const { return myWindowHandle; }
	[[nodiscard]] SIZE FORCEINLINE GetWindowSize() const { return myWindowSize; }



	FORCEINLINE std::shared_ptr<Shader> GetDefaultVSShader() const { return defaultVS; }
	FORCEINLINE std::shared_ptr<Shader> GetDefaultPSShader() const { return defaultPS; }

	FORCEINLINE ComPtr<ID3D11VertexShader> GetQuadShader() const { return myScreenSpaceQuadShader; }
	FORCEINLINE ComPtr<ID3D11PixelShader> GetLuminanceShader() const { return luminancePass; }
	FORCEINLINE ComPtr<ID3D11PixelShader> GetSSAOShader() const { return ScreenSpaceAmbienceOcclusion; }
	FORCEINLINE ComPtr<ID3D11PixelShader> GetEdgeBlurShader() const { return EdgeBlur; }
	FORCEINLINE ComPtr<ID3D11PixelShader> GetTonemap() const { return TonemapPass; }
	FORCEINLINE ComPtr<ID3D11PixelShader> GetCopyShader() const { return copyShader; }
	FORCEINLINE ComPtr<ID3D11PixelShader> GetGaussShader() const { return gaussShader; }
	FORCEINLINE ComPtr<ID3D11PixelShader> GetBloomShader() const { return bloomShader; }
	FORCEINLINE ComPtr<ID3D11VertexShader> GetParticleVSShader() const { return particleVertexShader; }
	FORCEINLINE ComPtr<ID3D11GeometryShader> GetParticleGSShader() const { return particleGeometryShader; }
	FORCEINLINE ComPtr<ID3D11PixelShader> GetParticlePSShader() const { return particlePixelShader; }

	/*FORCEINLINE ComPtr<ID3D11DeviceChild> GetShader(eShader type) const
	{
		switch(type)
		{
		case eShader::defaultVS:
			return myVertexShader.As<ID3D11DeviceChild>();

		case eShader::defaultPS:
			return myPixelShader.Get();

		case eShader::particleVS:
			return particleVertexShader.Get();

		case eShader::particleGS:
			return particleGeometryShader.Get();

		case eShader::particlePS:
			return particlePixelShader.Get();

		case eShader::screenSpaceQuad:
			return myScreenSpaceQuadShader.Get();

		case eShader::luminancePass:
			return luminancePass;

		case eShader::linearGammaPass:
			return linearGammaPass.Get();

		case eShader::copyShader:
			return copyShader.Get();

		case eShader::gaussShader:
			return gaussShader.Get();

		case eShader::bloomShader:
			return bloomShader.Get();

		case eShader::debugLineVS:
			return debugLineVS.Get();

		case eShader::debugLinePS:
			return debugLinePS.Get();
		}
	}
	*/

	FORCEINLINE std::shared_ptr<Texture> GetTargetTextures(eRenderTargets type) const
	{
		switch (type)
		{
		case::eRenderTargets::BackBuffer:
			return myBackBuffer;
		case::eRenderTargets::DepthBuffer:
			return myDepthBuffer;
		case::eRenderTargets::SceneBuffer:
			return SceneBuffer;
		case::eRenderTargets::halfSceneBuffer:
			return halfSceneBuffer;
		case::eRenderTargets::quaterSceneBuffer1:
			return quaterSceneBuffer1;
		case::eRenderTargets::quaterSceneBuffer2:
			return quaterSceneBuffer2;
		case::eRenderTargets::IntermediateA:
			return IntermediateA;
		case::eRenderTargets::IntermediateB:
			return IntermediateB;
		case::eRenderTargets::SSAO:
			return SSAOTexture;
		case::eRenderTargets::NoiseTexture:
			return NoiseTable->GetRawTexture();
		default:
			return nullptr;
		}
	}

	FORCEINLINE InstanceRenderer& GetInstanceRenderer() { return myInstanceRenderer; }
	FORCEINLINE GraphicsSettings& GetSettings() { return myGraphicSettings; }
	FORCEINLINE std::shared_ptr< Shader> GetDebugLineVS() const { return debugLineVS; }
	FORCEINLINE std::shared_ptr< Shader> GetDebugLinePS() const { return debugLinePS; }

	FORCEINLINE ComPtr<ID3D11BlendState> GetAlphaBlendState() const { return AlphaBlendState; }
	FORCEINLINE ComPtr<ID3D11BlendState> GetAdditiveBlendState() const { return AdditiveBlendState; }

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