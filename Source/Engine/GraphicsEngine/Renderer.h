#pragma once
#include "DebugDrawer/DebugDrawer.h"
#include "Engine/AssetManager/Enums.h"
#include "Tools/Utilities/System/SingletonTemplate.h"
 
class Camera;
class Texture;
class TextureHolder;
class CommandList;
class GameObjectManager;
class Viewport;
class Scene;
class Mesh;
class Material;
class PSOCache;
class ShipyardShader;

#define RENDERER ServiceLocator::Instance().GetService<Renderer>()

class Renderer: public Singleton {
	friend class GraphicsEngineUtilities;

public:
	enum class eDepthStencilStates: unsigned int {
		DSS_ReadWrite,
		DSS_ReadOnly,
		DSS_COUNT,
	};

	const PSOCache& GetPSOCache() const;

	bool ResizeBuffers(Vector2ui resolution);

	std::shared_ptr<PSOCache> m_Cache;
	std::shared_ptr<Texture>        m_DepthBuffer{};
	DebugDrawer               debugDrawer;

	float framerate = 60.f;
private:

	Vector2ui renderResolution;
	std::chrono::steady_clock::time_point start;
	std::chrono::steady_clock::time_point stop;
	uint32_t* BufferForPicking;
	bool      WantPickingData = false;

	Camera* myCamera;

	std::shared_ptr<ShipyardShader> defaultVS;
	std::shared_ptr<ShipyardShader> defaultPS;

	std::shared_ptr<Texture>       BRDLookUpTable;
	std::shared_ptr<TextureHolder> NoiseTable;
	std::shared_ptr<TextureHolder> defaultTexture;
	std::shared_ptr<TextureHolder> defaultNormalTexture;
	std::shared_ptr<TextureHolder> defaultMatTexture;
	std::shared_ptr<TextureHolder> defaultEffectTexture;
	std::shared_ptr<TextureHolder> defaultParticleTexture;
	std::shared_ptr<TextureHolder> defaultCubeMap;
	std::shared_ptr<Mesh>          defaultMesh;
	std::shared_ptr<Material>      defaultMaterial;

	uint32_t         ReadPickingData(Vector2ui position);

	std::shared_ptr<Scene>                 newScene;
	std::vector<std::shared_ptr<Viewport>> m_CustomSceneRenderPasses; // lifetime 1 frame

	void     AddRenderJob(std::shared_ptr<Viewport> aViewport);
	uint32_t GetAmountOfRenderJob();

	bool SetupDebugDrawline();
	void SetupDefaultVariables();
	void Init_brdfLUT();

	void     BeginFrame();
	uint64_t RenderFrame(Viewport& renderViewPort,GameObjectManager& scene);
	void     EndFrame();

	void PrepareBuffers(std::shared_ptr<CommandList> commandList,Viewport& renderViewPort,GameObjectManager& scene);
	void EnvironmentLightPass(std::shared_ptr<CommandList> commandList) const;
	void ToneMapperPass(std::shared_ptr<CommandList> commandList,Texture* target) const;
	void ImGuiPass();

public:
	bool Initialize(bool enableDeviceDebug);
	bool InitializeImguiBackends() const;
	void InitializeCustomRenderScene();
	void Render(std::vector<std::shared_ptr<Viewport>> renderViewPorts);
	void Update(float delta);
	void Shutdown();


	__forceinline std::shared_ptr<ShipyardShader> GetDefaultVSShader() const {
		return defaultVS;
	}

	__forceinline std::shared_ptr<ShipyardShader> GetDefaultPSShader() const {
		return defaultPS;
	}
	 
	__forceinline std::shared_ptr<Material> GetDefaultMaterial() const {
		return defaultMaterial;
	}

	__forceinline std::shared_ptr<TextureHolder> GetDefaultTexture(eTextureType type) const {
		switch(type) {
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
