#pragma once

#include "DirectX/Shipyard/Gpu_fwd.h"
#include "DirectX/Shipyard/PSO.h"
#include "Engine/AssetManager/Enums.h"
#include "Rendering/Buffers/FrameBuffer.h"
#include "Rendering/Buffers/GraphicSettingsBuffer.h"
#include "Rendering/Buffers/LightBuffer.h"
#include "Rendering/Buffers/LineBuffer.h"
#include "Rendering/Buffers/ObjectBuffer.h"
#include <Engine/AssetManager/ComponentSystem/GameObjectManager.h>
#include <Tools/Utilities/System/ServiceLocator.h>
#include <Tools/Utilities/System/SingletonTemplate.h>

// #define _DEBUGDRAW

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

class Camera;
class Texture;
class TextureHolder;
class CommandList;
class Scene;

#define GraphicsEngineInstance ServiceLocator::Instance().GetService<GraphicsEngine>()

class GraphicsEngine : public Singleton
{
    friend class GraphicsEngineUtilities;

  public:
    enum class eDepthStencilStates : unsigned int
    {
        DSS_ReadWrite,
        DSS_ReadOnly,
        DSS_COUNT,
    };
    const PSOCache &GetPSOCache() const;

    bool ResizeBuffers(Vector2ui resolution);

  private:
    FrameBuffer myFrameBuffer;
    PSOCache m_Cache;

    uint32_t *BufferForPicking;
    bool WantPickingData = false;

    Camera *myCamera;

    std::shared_ptr<Texture> SceneBuffer;
    std::shared_ptr<ShipyardShader> defaultVS;
    std::shared_ptr<ShipyardShader> defaultPS;

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

    GraphicsSettings myGraphicSettings;
    HeapHandle ViewPortHeapHandle;
    uint32_t ReadPickingData(Vector2ui position);

    std::shared_ptr<Scene> newScene;
    std::vector<std::shared_ptr<Viewport>> m_CustomSceneRenderPasses; // lifetime 1 frame

    void AddRenderJob(std::shared_ptr<Viewport> aViewport);
    uint32_t GetAmountOfRenderJob();

  private:
    bool SetupDebugDrawline();
    void SetupDefaultVariables();
    void SetupBlendStates();
    void SetupParticleShaders();
    void UpdateSettings();
    void SetupSpace3();
    void SetupPostProcessing();

    void BeginFrame();
    uint64_t RenderFrame(Viewport &renderViewPort, GameObjectManager &scene);
    void EndFrame();

    void PrepareBuffers(std::shared_ptr<CommandList> commandList, Viewport &renderViewPort, GameObjectManager &scene);
    void DeferredRenderingPass(std::shared_ptr<CommandList> commandList, Viewport &renderViewPort,
                               GameObjectManager &scene);
    void EnvironmentLightPass(std::shared_ptr<CommandList> commandList);
    void ToneMapperPass(std::shared_ptr<CommandList> commandList, Texture *target);
    void ImGuiPass();

  public:
    bool Initialize(HWND windowHandle, bool enableDeviceDebug);
    void InitializeCustomRenderScene();
    void Render(std::vector<std::shared_ptr<Viewport>> renderViewPorts);

    void SetDepthState(eDepthStencilStates state)
    {
        state;
        // RHI::Context->OMSetDepthStencilState(myDepthStencilStates[(int)state].Get(),0);
    }

    FORCEINLINE std::shared_ptr<ShipyardShader> GetDefaultVSShader() const
    {
        return defaultVS;
    }
    FORCEINLINE std::shared_ptr<ShipyardShader> GetDefaultPSShader() const
    {
        return defaultPS;
    }
    FORCEINLINE std::shared_ptr<Texture> GetTargetTextures(eRenderTargets type) const;

    FORCEINLINE GraphicsSettings &GetSettings()
    {
        return myGraphicSettings;
    }

    FORCEINLINE std::shared_ptr<Material> GetDefaultMaterial() const
    {
        return defaultMaterial;
    }

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
