#pragma once
#include <memory> 
#include <wrl.h>

#include <Engine/GraphicsEngine/GraphicCommands/GraphicCommands.h>
#include <Engine/GraphicsEngine/Shaders/Registers.h>

#include "Rendering/Buffers/FrameBuffer.h"
#include "Rendering/Buffers/ObjectBuffer.h"
#include "Rendering/Buffers/LineBuffer.h"
#include "Rendering/Buffers/G_Buffer.h"
#include "Rendering/Buffers/ConstantBuffer.h" 

#include "InterOp/RHI.h"
#include "Rendering/ShadowRenderer/ShadowRenderer.h"

#include <Engine/AssetManager/Objects/BaseAssets/TextureAsset.h>
#include <Engine/AssetManager/Objects/BaseAssets/MeshAsset.h>

#include <Tools/ThirdParty/CU/CommonUtills/Matrix4x4.hpp> 


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
	count
};


using namespace Microsoft::WRL;

class cCamera;

class GraphicsEngine
{
	friend class GraphicCommandBase;
	friend class ShadowRenderer;
private:
	FrameBuffer myFrameBuffer;
	ObjectBuffer myObjectBuffer;
	LineBuffer myLineBuffer;
	LightBuffer myLightBuffer;
	G_Buffer myG_Buffer;
	//Fill with render data for the deffered pass
	GraphicsCommandList DeferredCommandList;
	GraphicsCommandList OverlayCommandList;

	SIZE myWindowSize{0,0};
	HWND myWindowHandle{};
	cCamera* myCamera;
	CU::Vector4<float> myBackgroundColor;

	std::shared_ptr<Texture> myBackBuffer;
	std::shared_ptr<Texture> myDepthBuffer;

	std::shared_ptr<Texture> SceneBuffer;
	std::shared_ptr<Texture> halfSceneBuffer;
	std::shared_ptr<Texture> quaterSceneBuffer1;
	std::shared_ptr<Texture> quaterSceneBuffer2;

	std::shared_ptr<Texture> IntermediateA;
	std::shared_ptr<Texture> IntermediateB;



	//Defualtl
	ComPtr<ID3D11VertexShader> myVertexShader;
	ComPtr<ID3D11PixelShader> myPixelShader;
	std::shared_ptr<Shader> defaultVS;
	std::shared_ptr<Shader> defaultPS;


	//Post-pro
	ComPtr<ID3D11VertexShader> myScreenSpaceQuadShader;
	ComPtr<ID3D11PixelShader> luminancePass;
	ComPtr<ID3D11PixelShader> linearGammaPass;
	ComPtr<ID3D11PixelShader> copyShader;
	ComPtr<ID3D11PixelShader> gaussShader;
	ComPtr<ID3D11PixelShader> bloomShader;






	//Debug
	ComPtr<ID3D11Buffer> myLineVertexBuffer;
	ComPtr<ID3D11Buffer> myLineIndexBuffer;
	std::shared_ptr<Shader> debugLinePS;
	std::shared_ptr<Shader> debugLineVS;

	std::shared_ptr<Texture> BRDLookUpTable;
	std::shared_ptr<TextureHolder> defaultTexture;
	std::shared_ptr<TextureHolder> defaultNormalTexture;
	std::shared_ptr<TextureHolder> defaultMatTexture;
	std::shared_ptr<TextureHolder> defaultEffectTexture;
	std::shared_ptr<TextureHolder> defaultCubeMap;





	std::shared_ptr<Mesh> defaultMesh;
	std::shared_ptr<Material> defaultMaterial;

	ComPtr<ID3D11SamplerState> myDefaultSampleState;
	ComPtr<ID3D11SamplerState> myShadowSampleState;
	ComPtr<ID3D11SamplerState> myBRDFSampleState;

	ComPtr<ID3D11BlendState> AlphaBlendState;
	ComPtr<ID3D11BlendState> AdditiveBlendState;

	ShadowRenderer myShadowRenderer;
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

	bool SetupDebugDrawline(bool& retFlag);

	void SetupDefaultVariables();

	void SetupBRDF();

	void SetupPostProcessing();

	void SetLoggingWindow(HANDLE aHandle);

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
	CU::Vector4<float>& GetBackgroundColor() { return myBackgroundColor; }

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
	FORCEINLINE ComPtr<ID3D11PixelShader> GetLinearToGamma() const { return linearGammaPass; }
	FORCEINLINE ComPtr<ID3D11PixelShader> GetCopyShader() const { return copyShader; }
	FORCEINLINE ComPtr<ID3D11PixelShader> GetGaussShader() const { return gaussShader; }
	FORCEINLINE ComPtr<ID3D11PixelShader> GetBloomShader() const { return bloomShader; }

	FORCEINLINE std::shared_ptr<Texture> GetTargetTextures(eRenderTargets type) const
	{
		switch(type)
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
		default:
			return nullptr;
		}
	}

	Texture* DepthBuffer()const { return myDepthBuffer.get(); };
	Texture* BackBuffer()const { return myBackBuffer.get(); };

	FORCEINLINE std::shared_ptr<Shader> GetDebugLineVS() const { return debugLineVS; }
	FORCEINLINE std::shared_ptr<Shader> GetDebugLinePS() const { return debugLinePS; }

	FORCEINLINE ComPtr<ID3D11BlendState> GetAlphaBlendState() const { return AlphaBlendState; }
	FORCEINLINE ComPtr<ID3D11BlendState> GetAdditiveBlendState() const { return AdditiveBlendState; }

	FORCEINLINE std::shared_ptr<TextureHolder> GetDefaultTexture() const { return defaultTexture; }
	FORCEINLINE std::shared_ptr<Material> GetDefaultMaterial() const { return defaultMaterial; }
	FORCEINLINE std::shared_ptr<TextureHolder> GetDefaultTexture(eTextureType type) const
	{
		switch(type)
		{
		case eTextureType::ColorMap:
			return defaultTexture;

		case eTextureType::NormalMap:
			return defaultNormalTexture;

		case eTextureType::MaterialMap:
			return defaultMatTexture;

		case eTextureType::EffectMap:
			return defaultEffectTexture;

		case eTextureType::CubeMap:
			return defaultCubeMap;

		default:
			return defaultTexture;
		}
	}
};