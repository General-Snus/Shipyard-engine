#include "Engine/GraphicsEngine/GraphicsEngine.pch.h"

#include "DirectX/Shipyard/CommandList.h"
#include <DirectX/XTK/DDSTextureLoader.h>
#include <DirectX/directx/d3dx12_pipeline_state_stream.h>
#include <DirectX/directx/d3dx12_root_signature.h>
#include <Engine/AssetManager/AssetManager.h>
#include <Engine/AssetManager/ComponentSystem/Components/CameraComponent.h>
#include <Engine/AssetManager/ComponentSystem/Components/LightComponent.h>
#include <Engine/AssetManager/Objects/BaseAssets/MaterialAsset.h>
#include <Engine/AssetManager/Objects/BaseAssets/MeshAsset.h>
#include <Engine/AssetManager/Objects/BaseAssets/TextureAsset.h>
#include <d3dcompiler.h>

#include "DirectX/Shipyard/GPU.h"
#include "DirectX/Shipyard/Helpers.h"
#include "DirectX/Shipyard/PSO.h"
#include "Editor/Editor/Windows/Window.h"
#include "Engine/AssetManager/ComponentSystem/Components/MeshRenderer.h"
#include "Engine/AssetManager/ComponentSystem/Components/Transform.h"
#include "Engine/AssetManager/Objects/BaseAssets/ShipyardShader.h"
#include "Engine/PersistentSystems/Scene.h"
#include "Rendering/Passes.h"
#include "Tools/ImGui/ImGui/ImGuizmo.h"
#include "Tools/ImGui/ImGui/backends/imgui_impl_dx12.h"
#include "Tools/ImGui/ImGui/imgui_internal.h"
#include "Tools/Utilities/Input/Input.hpp"
#include <Editor/Editor/Windows/EditorWindows/Viewport.h>

bool GraphicsEngine::Initialize(HWND windowHandle, bool enableDeviceDebug)
{
    myWindowHandle = windowHandle;
    if (!GPU::Initialize(myWindowHandle, enableDeviceDebug, Window::Width(), Window::Height()))
    {
        Logger::Err("Failed to initialize the DX12 GPU!");
        return false;
    }

    PSOCache::InitRootSignature();
    PSOCache::InitAllStates();

    SetupDefaultVariables();
    SetupSpace3();
    SetupParticleShaders();
    SetupPostProcessing();
    SetupBlendStates();
    SetupDebugDrawline();

    InitializeCustomRenderScene();

    const auto &gBufferTextures = PSOCache::GetState(PSOCache::ePipelineStateID::GBuffer)->RenderTargets();

    const size_t dataSize = gBufferTextures[5].GetWidth() * gBufferTextures[5].GetHeight() * sizeof(float) * 2;
    BufferForPicking = new uint32_t[dataSize];
    const auto commandQueue = GPU::GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
    OPTICK_GPU_INIT_D3D12(GPU::m_Device.Get(), commandQueue->GetCommandQueue().GetAddressOf(), 1);
    return true;
}

void GraphicsEngine::InitializeCustomRenderScene()
{
    newScene = std::make_shared<Scene>();
    {
        GameObject worldRoot = GameObject::Create("WordRoot", newScene);
        Transform &transform = worldRoot.AddComponent<Transform>();
        transform.SetRotation(80, 0, 0);
        transform.SetPosition(0, 5, 0);
        cLight &pLight = worldRoot.AddComponent<cLight>(eLightType::Directional);
        pLight.SetColor(Vector3f(1, 1, 1));
        pLight.SetPower(2.0f);
        pLight.BindDirectionToTransform(true);
    }

    const auto res = Vector2f(1920.f, 1080.f);
    {
        GameObject camera = GameObject::Create("Camera", newScene);
        auto &cameraComponent = camera.AddComponent<cCamera>();
        cameraComponent.SetResolution(res);

        newScene->GetGOM().SetLastGOAsCamera();
        cameraComponent.SetActive(true);
    }

    {
        auto renderObject = GameObject::Create("RenderMesh", newScene);
        renderObject.AddComponent<Transform>();
        renderObject.AddComponent<cMeshRenderer>();
        newScene->GetGOM().SetLastGOAsPlayer();
    }
}

uint32_t GraphicsEngine::ReadPickingData(Vector2ui position)
{
    const int pos = position.x * position.y;

    return (unsigned)pos;
}

bool GraphicsEngine::SetupDebugDrawline()
{
    DebugDrawer::Get().Initialize();
    DebugDrawer::Get().AddDebugGrid({0.f, 0.0f, 0.f}, 1000, 10, {1.0f, 1.0f, 1.0f});
    return true;
}
void GraphicsEngine::SetupDefaultVariables()
{

    ////Particle
    AssetManager::Get().ForceLoadAsset<TextureHolder>(L"Textures/Default/DefaultParticle_P.dds",
                                                      defaultParticleTexture);
    defaultParticleTexture->SetTextureType(eTextureType::ParticleMap);

    AssetManager::Get().ForceLoadAsset<TextureHolder>(L"Textures/Default/NoiseTable.dds", NoiseTable);

    AssetManager::Get().ForceLoadAsset<ShipyardShader>("Shaders/Default_VS.cso", defaultVS);
    AssetManager::Get().ForceLoadAsset<ShipyardShader>("Shaders/Default_PS.cso", defaultPS);
    AssetManager::Get().ForceLoadAsset<Material>("Materials/Default.json", defaultMaterial);
    defaultMaterial->SetShader(defaultVS, defaultPS);

    AssetManager::Get().ForceLoadAsset<TextureHolder>("Textures/Default/DefaultTile.dds", defaultTexture);
    defaultTexture->SetTextureType(eTextureType::ColorMap);
    AssetManager::Get().ForceLoadAsset<TextureHolder>("Textures/Default/DefaultNormal.dds", defaultNormalTexture);
    defaultNormalTexture->SetTextureType(eTextureType::NormalMap);
    AssetManager::Get().ForceLoadAsset<TextureHolder>("Textures/Default/DefaultMaterial.dds", defaultMatTexture);
    defaultMatTexture->SetTextureType(eTextureType::MaterialMap);
    AssetManager::Get().ForceLoadAsset<TextureHolder>("Textures/Default/DefaultEffect.dds", defaultEffectTexture);
    defaultEffectTexture->SetTextureType(eTextureType::EffectMap);

    AssetManager::Get().ForceLoadAsset<Mesh>("default.fbx", defaultMesh);
}
void GraphicsEngine::SetupBlendStates()
{
    /*D3D11_BLEND_DESC blendDesc = {};
    D3D11_RENDER_TARGET_BLEND_DESC& rtBlendDesc = blendDesc.RenderTarget[0];
    rtBlendDesc.BlendEnable = TRUE;
    rtBlendDesc.SrcBlend = D3D11_BLEND_ONE;
    rtBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    rtBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
    rtBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
    rtBlendDesc.DestBlendAlpha = D3D11_BLEND_ONE;
    rtBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    rtBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    if (!RHI::CreateBlendState(AlphaBlendState,blendDesc))
    {
        assert(false);
    }

    rtBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
    rtBlendDesc.DestBlend = D3D11_BLEND_ONE;
    rtBlendDesc.SrcBlendAlpha = D3D11_BLEND_ZERO;
    rtBlendDesc.DestBlendAlpha = D3D11_BLEND_ONE;
    rtBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    rtBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    if (!RHI::CreateBlendState(AdditiveBlendState,blendDesc))
    {
        assert(false);
    }*/
}
void GraphicsEngine::SetupSpace3()
{
    const auto commandQueue = GPU::GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
    const auto commandList = commandQueue->GetCommandList(L"RenderFrame");
    const auto graphicCommandList = commandList->GetGraphicsCommandList();

    // Light
    AssetManager::Get().ForceLoadAsset<TextureHolder>("Textures/skansen_cubemap.dds", defaultCubeMap);
    defaultCubeMap->SetTextureType(eTextureType::CubeMap);

    const Vector2ui size = {512, 512};
    BRDLookUpTable = std::make_shared<Texture>();
    BRDLookUpTable->AllocateTexture(size, L"brdfLUT", DXGI_FORMAT_R16G16_FLOAT);
    BRDLookUpTable->SetView(ViewType::SRV);
    commandList->SetRenderTargets(1, BRDLookUpTable.get(), nullptr);

    constexpr std::array rt = {DXGI_FORMAT_R16G16_FLOAT};
    const auto brdfPSO = PSO::CreatePSO("Shaders/ScreenspaceQuad_VS.cso", "Shaders/brdfLUT_PS.cso", rt);

    const D3D12_VIEWPORT viewPort = {
        0.0f, 0.0f, static_cast<float>(size.x), static_cast<float>(size.y), D3D12_MIN_DEPTH, D3D12_MAX_DEPTH};
    const D3D12_RECT rect = {0, 0, static_cast<LONG>(size.x), static_cast<LONG>(size.y)};

    graphicCommandList->RSSetViewports(1, &viewPort);
    graphicCommandList->RSSetScissorRects(1, &rect);

    const auto &rootSignature = PSOCache::m_RootSignature->GetRootSignature();
    graphicCommandList->SetGraphicsRootSignature(rootSignature.Get());
    commandList->TrackResource(rootSignature);

    graphicCommandList->SetPipelineState(brdfPSO->GetPipelineState().Get());

    graphicCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    graphicCommandList->IASetVertexBuffers(0, 1, nullptr);
    graphicCommandList->IASetIndexBuffer(nullptr);
    graphicCommandList->DrawInstanced(6, 1, 0, 0);

    const auto fence = commandQueue->ExecuteCommandList(commandList);
    commandQueue->WaitForFenceValue(fence);
}
void GraphicsEngine::SetupPostProcessing()
{
    // D3D11_SAMPLER_DESC normalDepthSampler = {};
    // normalDepthSampler.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
    // normalDepthSampler.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    // normalDepthSampler.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    // normalDepthSampler.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

    // if (!RHI::CreateSamplerState(myNormalDepthSampleState,normalDepthSampler))
    //{
    //	Logger::Log("Sampler state created");
    //	assert(false);
    // }

    // RHI::SetSamplerState(myNormalDepthSampleState,REG_normalDepthSampler);

    // RHI::DeviceSize size = RHI::GetDeviceSize();
    // SceneBuffer = std::make_shared<Texture>();
    // RHI::CreateTexture(
    //	SceneBuffer.get(),
    //	L"SceneBuffer",
    //	size.Width,size.Height,
    //	defaultTextureFormat,
    //	D3D11_USAGE_DEFAULT,
    //	D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
    //	0
    //);

    // halfSceneBuffer = std::make_shared<Texture>();
    // RHI::CreateTexture(
    //	halfSceneBuffer.get(),
    //	L"halfSceneBuffer",
    //	size.Width / 2,size.Height / 2,
    //	defaultTextureFormat,
    //	D3D11_USAGE_DEFAULT,
    //	D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
    //	0
    //);
    // quaterSceneBuffer1 = std::make_shared<Texture>();
    // RHI::CreateTexture(
    //	quaterSceneBuffer1.get(),
    //	L"quaterSceneBuffer1",
    //	size.Width / 4,size.Height / 4,
    //	defaultTextureFormat,
    //	D3D11_USAGE_DEFAULT,
    //	D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
    //	0
    //);
    // quaterSceneBuffer2 = std::make_shared<Texture>();
    // RHI::CreateTexture(
    //	quaterSceneBuffer2.get(),
    //	L"quaterSceneBuffer2",
    //	size.Width / 4,size.Height / 4,
    //	defaultTextureFormat,
    //	D3D11_USAGE_DEFAULT,
    //	D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
    //	0
    //);
    // IntermediateA = std::make_shared<Texture>();
    // RHI::CreateTexture(
    //	IntermediateA.get(),
    //	L"IntermediateA",
    //	size.Width,size.Height,
    //	defaultTextureFormat,
    //	D3D11_USAGE_DEFAULT,
    //	D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
    //	0
    //);

    // IntermediateB = std::make_shared<Texture>();
    // RHI::CreateTexture(
    //	IntermediateB.get(),
    //	L"IntermediateB",
    //	size.Width,size.Height,
    //	defaultTextureFormat,
    //	D3D11_USAGE_DEFAULT,
    //	D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
    //	0
    //);

    // SSAOTexture = std::make_shared<Texture>();
    // RHI::CreateTexture(
    //	SSAOTexture.get(),
    //	L"SSAOTexture",
    //	size.Width / 2,size.Height / 2,
    //	defaultTextureFormat,
    //	D3D11_USAGE_DEFAULT,
    //	D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
    //	0
    //);
}
void GraphicsEngine::SetupParticleShaders()
{
    /*RHI::CreateVertexShaderAndInputLayout(
        particleVertexShader,
        Particlevertex::InputLayout,
        Particlevertex::InputLayoutDefinition,
        BuiltIn_ParticleShader_VS_ByteCode,
        sizeof(BuiltIn_ParticleShader_VS_ByteCode)
    );
    RHI::CreateGeometryShader(
        particleGeometryShader,
        BuiltIn_ParticleShader_GS_ByteCode,
        sizeof(BuiltIn_ParticleShader_GS_ByteCode)
    );
    RHI::CreatePixelShader(
        particlePixelShader,
        BuiltIn_ParticleShader_PS_ByteCode,
        sizeof(BuiltIn_ParticleShader_PS_ByteCode)
    );*/
}
void GraphicsEngine::UpdateSettings()
{
    myGraphicSettingsBuffer.GSB_ToneMap = myGraphicSettings.Tonemaptype;
    myGraphicSettingsBuffer.GSB_AO_intensity = 0.35f;
    myGraphicSettingsBuffer.GSB_AO_scale = 0.05f;
    myGraphicSettingsBuffer.GSB_AO_bias = 0.5f;
    myGraphicSettingsBuffer.GSB_AO_radius = 0.002f;
    myGraphicSettingsBuffer.GSB_AO_offset = 0.707f;
    /*
    RHI::SetConstantBuffer(PIPELINE_STAGE_PIXEL_SHADER,REG_GraphicSettingsBuffer,myGraphicSettingsBuffer);
    RHI::UpdateConstantBufferData(myGraphicSettingsBuffer);
    */
}

void GraphicsEngine::AddRenderJob(std::shared_ptr<Viewport> aViewport)
{
    m_CustomSceneRenderPasses.emplace_back(aViewport);
}

uint32_t GraphicsEngine::GetAmountOfRenderJob()
{
    return static_cast<uint32_t>(m_CustomSceneRenderPasses.size());
}

void GraphicsEngine::Render(std::vector<std::shared_ptr<Viewport>> renderViewPorts)
{
    OPTICK_EVENT();

    for (auto &viewport : m_CustomSceneRenderPasses)
    {
        renderViewPorts.emplace_back(viewport);
        viewport->Update();
    }

    BeginFrame();

    for (auto &viewport : renderViewPorts)
    {
        RenderFrame(*viewport, viewport->sceneToRender->GetGOM());
    }
    EndFrame();

    for (auto &viewport : m_CustomSceneRenderPasses)
    {
        viewport->m_RenderTarget->isBeingLoaded = false;
        viewport->m_RenderTarget->isLoadedComplete = true;
    }
    m_CustomSceneRenderPasses.clear();
}

void GraphicsEngine::BeginFrame()
{
    OPTICK_EVENT();
    myCamera = Scene::ActiveManager().GetCamera().TryGetComponent<cCamera>();
    if (!myCamera)
    {
        Logger::Err("No camera in scene. No render is possible");
    }
    UpdateSettings();
}

uint64_t GraphicsEngine::RenderFrame(Viewport &renderViewPort, GameObjectManager &scene)
{
    if (renderViewPort.IsRenderReady())
    {
        OPTICK_EVENT();
        auto commandQueue = GPU::GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
        auto commandList = commandQueue->GetCommandList(L"RenderFrame");
        OPTICK_GPU_CONTEXT(commandList->GetGraphicsCommandList().Get());

        OPTICK_GPU_EVENT("RenderFrame");
        PrepareBuffers(commandList, renderViewPort, scene);
        Passes::WriteShadows(commandList, scene);
        commandList->FlushResourceBarriers();

        DeferredRenderingPass(commandList, renderViewPort, scene);
        EnvironmentLightPass(commandList);
        ToneMapperPass(commandList, renderViewPort.GetTarget());
        return commandQueue->ExecuteCommandList(commandList);
    }
    else
    {
        auto commandQueue = GPU::GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
        auto commandList = commandQueue->GetCommandList(L"RenderFrame");
        GPU::ClearRTV(*commandList, renderViewPort.GetTarget());
        return commandQueue->ExecuteCommandList(commandList);
    }
}

void GraphicsEngine::EndFrame()
{
    OPTICK_GPU_EVENT("EndFrame");
    ImGuiPass();
    GPU::Present();
}

void GraphicsEngine::PrepareBuffers(std::shared_ptr<CommandList> commandList, Viewport &renderViewPort,
                                    GameObjectManager &scene)
{
    OPTICK_GPU_EVENT("PrepareBuffers");

    // const UINT currentBackBufferIndex = chain->GetCurrentBackBufferIndex();
    const auto *backBuffer = GPU::GetCurrentBackBuffer();
    const auto rtv = GPU::GetCurrentRenderTargetView();
    const auto dsv = GPU::m_DepthBuffer->GetHandle(ViewType::DSV).cpuPtr;
    commandList->TransitionBarrier(backBuffer->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET);
    const Vector4f clearColor = {0.0f, 0.0f, 0.0f, 0.0f};
    GPU::ClearRTV(*commandList.get(), rtv, clearColor);
    GPU::ClearDepth(*commandList.get(), GPU::m_DepthBuffer->GetHandle(ViewType::DSV).cpuPtr);

    const auto &rootSignature = PSOCache::m_RootSignature->GetRootSignature();
    commandList->GetGraphicsCommandList()->SetGraphicsRootSignature(rootSignature.Get());
    commandList->TrackResource(rootSignature);

    ID3D12DescriptorHeap *heaps[] = {
        GPU::m_ResourceDescriptors[static_cast<int>(eHeapTypes::HEAP_TYPE_CBV_SRV_UAV)]->Heap(),
        GPU::m_ResourceDescriptors[static_cast<int>(eHeapTypes::HEAP_TYPE_SAMPLER)]->Heap()};
    commandList->GetGraphicsCommandList()->SetDescriptorHeaps((UINT)std::size(heaps), heaps);

    {
        const LightBuffer lightBuffer = Passes::CreateLightBuffer(scene);
        const GraphicsResource &alloc = GPU::m_GraphicsMemory->AllocateConstant<LightBuffer>(lightBuffer);
        commandList->GetGraphicsCommandList()->SetGraphicsRootConstantBufferView(REG_LightBuffer, alloc.GpuAddress());

        const auto frameBuffer = renderViewPort.GetCamera().GetFrameBuffer();
        const auto &alloc0 = GPU::m_GraphicsMemory->AllocateConstant<FrameBuffer>(frameBuffer);
        commandList->GetGraphicsCommandList()->SetGraphicsRootConstantBufferView((int)eRootBindings::frameBuffer,
                                                                                 alloc0.GpuAddress());

        const auto cubeMap = defaultCubeMap->GetRawTexture().get();
        commandList->SetDescriptorTable((int)eRootBindings::PermanentTextures, cubeMap);
        commandList->TrackResource(cubeMap->GetResource());

        commandList->GetGraphicsCommandList()->SetGraphicsRootDescriptorTable(
            (int)eRootBindings::Textures,
            GPU::m_ResourceDescriptors[static_cast<int>(eHeapTypes::HEAP_TYPE_CBV_SRV_UAV)]->GetFirstGpuHandle());
        commandList->GetGraphicsCommandList()->SetGraphicsRootDescriptorTable(
            (int)eRootBindings::MeshBuffer,
            GPU::m_ResourceDescriptors[static_cast<int>(eHeapTypes::HEAP_TYPE_CBV_SRV_UAV)]->GetFirstGpuHandle());
    }
}
void GraphicsEngine::DeferredRenderingPass(std::shared_ptr<CommandList> commandList, Viewport &renderViewPort,
                                           GameObjectManager &scene)
{
    OPTICK_GPU_EVENT("DeferredRenderingPass");
    auto &list = scene.GetAllComponents<cMeshRenderer>();
    {
        constexpr uint32_t bufferCount = 7;
        commandList->GetGraphicsCommandList()->RSSetViewports(1, &GPU::m_Viewport);
        commandList->GetGraphicsCommandList()->RSSetScissorRects(1, &GPU::m_ScissorRect);

        const auto &gbufferPSO = PSOCache::GetState(PSOCache::ePipelineStateID::GBuffer);
        const auto &gBufferTextures = gbufferPSO->RenderTargets();
        GPU::ClearRTV(*commandList.get(), gBufferTextures, bufferCount);
        commandList->SetRenderTargets(bufferCount, gBufferTextures, GPU::m_DepthBuffer.get());

        const auto &pipelineState = gbufferPSO->GetPipelineState().Get();
        commandList->GetGraphicsCommandList()->SetPipelineState(pipelineState);
        commandList->TrackResource(pipelineState);

        auto frameBuffer = renderViewPort.GetCamera().GetFrameBuffer();
        const auto &alloc0 = GPU::m_GraphicsMemory->AllocateConstant<FrameBuffer>(frameBuffer);
        commandList->GetGraphicsCommandList()->SetGraphicsRootConstantBufferView((int)eRootBindings::frameBuffer,
                                                                                 alloc0.GpuAddress());
    }

    const uint32_t dtAlbedo = defaultTexture->GetRawTexture()->GetHeapOffset();
    const uint32_t dtNormal = defaultNormalTexture->GetRawTexture()->GetHeapOffset();
    const uint32_t dtMaterial = defaultMatTexture->GetRawTexture()->GetHeapOffset();
    const uint32_t dtEffect = defaultEffectTexture->GetRawTexture()->GetHeapOffset();

    int vertCount = 0;
    for (auto &meshRenderer : list)
    {
        if (!meshRenderer.IsActive())
        {
            continue;
        }

        const auto ID = meshRenderer.GetOwner();
        const auto &transform = meshRenderer.GetComponent<Transform>();

        ObjectBuffer objectBuffer;
        objectBuffer.myTransform = transform.WorldMatrix();
        objectBuffer.MaxExtents = meshRenderer.GetRawMesh()->Bounds.GetMax();
        objectBuffer.MinExtents = meshRenderer.GetRawMesh()->Bounds.GetMin();
        objectBuffer.hasBone = false;
        objectBuffer.uniqueID = ID;
        commandList->AllocateBuffer(eRootBindings::objectBuffer, objectBuffer);

        for (auto &element : meshRenderer.GetElements())
        {
            vertCount += element.VertexBuffer.GetVertexCount();
            GPU::ConfigureInputAssembler(*commandList, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, element.IndexResource);

            const unsigned materialIndex = element.MaterialIndex;
            MaterialBuffer materialBuffer = meshRenderer.GetMaterial(materialIndex)->GetMaterialData();

            for (int i = 0; i < static_cast<int>(eTextureType::EffectMap) + 1; i++)
            {
                OPTICK_GPU_EVENT("SetTextures");
                if (auto textureAsset = meshRenderer.GetTexture(static_cast<eTextureType>(i), materialIndex))
                {
                    auto tex = textureAsset->GetRawTexture();
                    tex->SetView(ViewType::SRV);
                    commandList->TrackResource(tex->GetResource());
                    const auto heapOffset = tex->GetHeapOffset();
                    switch (static_cast<eTextureType>(i))
                    {
                    case eTextureType::ColorMap:
                        materialBuffer.albedoTexture = heapOffset != -1 ? heapOffset : dtAlbedo;
                        break;
                    case eTextureType::NormalMap:
                        materialBuffer.normalTexture = heapOffset != -1 ? heapOffset : dtNormal;
                        break;
                    case eTextureType::MaterialMap:
                        materialBuffer.materialTexture = heapOffset != -1 ? heapOffset : dtMaterial;
                        break;
                    case eTextureType::EffectMap:
                        materialBuffer.emissiveTexture = heapOffset != -1 ? heapOffset : dtEffect;
                        break;
                    default:
                        Logger::Critical("Texture type is not found or is not valid for used on deffered");
                        break;
                    }
                }
                else
                {
                    switch (static_cast<eTextureType>(i))
                    {
                    case eTextureType::ColorMap:
                        materialBuffer.albedoTexture = dtAlbedo;
                        break;
                    case eTextureType::NormalMap:
                        materialBuffer.normalTexture = dtNormal;
                        break;
                    case eTextureType::MaterialMap:
                        materialBuffer.materialTexture = dtMaterial;
                        break;
                    case eTextureType::EffectMap:
                        materialBuffer.emissiveTexture = dtEffect;
                        break;
                    default:
                        Logger::Critical("Texture type is not found or is not valid for used on deffered");
                        break;
                    }
                }
            }
            materialBuffer.vertexBufferIndex = element.VertexBuffer.GetHandle(ViewType::SRV).heapOffset;
            materialBuffer.vertexOffset =
                0; // vertex offset is part of drawcall, if i ever use this i need to set it here

            assert(materialBuffer.albedoTexture != -1 && "HEAP INDEX OUT OF BOUND");
            assert(materialBuffer.normalTexture != -1 && "HEAP INDEX OUT OF BOUND");
            assert(materialBuffer.materialTexture != -1 && "HEAP INDEX OUT OF BOUND");
            assert(materialBuffer.emissiveTexture != -1 && "HEAP INDEX OUT OF BOUND");
            assert(materialBuffer.vertexBufferIndex != -1 && "HEAP INDEX OUT OF BOUND");
            assert(materialBuffer.vertexOffset != -1 && "HEAP INDEX OUT OF BOUND");

            const auto &alloc2 = GPU::m_GraphicsMemory->AllocateConstant<MaterialBuffer>(materialBuffer);
            commandList->GetGraphicsCommandList()->SetGraphicsRootConstantBufferView(REG_DefaultMaterialBuffer,
                                                                                     alloc2.GpuAddress());

            OPTICK_GPU_EVENT("Draw");
            commandList->GetGraphicsCommandList()->DrawIndexedInstanced(element.IndexResource.GetIndexCount(), 1, 0, 0,
                                                                        0);
        }
    }
}
void GraphicsEngine::EnvironmentLightPass(std::shared_ptr<CommandList> commandList)
{
    OPTICK_GPU_EVENT("EnvironmentLightPass");
    auto &environmentLight = PSOCache::GetState(PSOCache::ePipelineStateID::DeferredLighting);
    constexpr uint32_t bufferCount = 7;
    const auto &gBufferTextures = PSOCache::GetState(PSOCache::ePipelineStateID::GBuffer)->RenderTargets();

    // TODO READBACK BUFFER
    /*
    if (true)
    {
        const auto& resourceRef = gBufferTextures[5].GetResource();
        const size_t dataSize = gBufferTextures[5].GetWidth() * gBufferTextures[5].GetHeight() * sizeof(float) * 2;
        void* mapData = BufferForPicking;
        D3D12_RANGE readRange = { 0,dataSize};
        HRESULT hr = resourceRef->Map(0,&readRange,&mapData);

        if (SUCCEEDED(hr))
        {
            memcpy(BufferForPicking,mapData,dataSize);
            resourceRef->Unmap(0,nullptr);
        }
        WantPickingData = false;
    }*/

    {
        GPU::ClearRTV(*commandList.get(), environmentLight->RenderTargets(), environmentLight->GetNumberOfTargets());
        commandList->SetRenderTargets(environmentLight->GetNumberOfTargets(), environmentLight->RenderTargets(),
                                      nullptr);

        const auto &pipelineState = environmentLight->GetPipelineState().Get();
        commandList->GetGraphicsCommandList()->SetPipelineState(pipelineState);
        commandList->TrackResource(pipelineState);

        for (unsigned i = 0; i < bufferCount; i++)
        {
            gBufferTextures[i].SetView(ViewType::SRV);
            commandList->TransitionBarrier(gBufferTextures[i], D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
            commandList->TrackResource(gBufferTextures[i]);
        }
        commandList->FlushResourceBarriers();
        commandList->SetDescriptorTable((int)eRootBindings::GbufferPasses, gBufferTextures);

        commandList->GetGraphicsCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->GetGraphicsCommandList()->IASetVertexBuffers(0, 1, nullptr);
        commandList->GetGraphicsCommandList()->IASetIndexBuffer(nullptr);
        commandList->GetGraphicsCommandList()->DrawInstanced(6, 1, 0, 0);
    }
}
void GraphicsEngine::ToneMapperPass(std::shared_ptr<CommandList> commandList, Texture *target)
{
    OPTICK_GPU_EVENT("ToneMapperPass");
    const auto &toneMapper = PSOCache::GetState(PSOCache::ePipelineStateID::ToneMap);

    const auto &pipelineState = toneMapper->GetPipelineState().Get();
    commandList->GetGraphicsCommandList()->SetPipelineState(pipelineState);
    commandList->TrackResource(pipelineState);

    auto *renderTargets = PSOCache::GetState(PSOCache::ePipelineStateID::DeferredLighting)->RenderTargets();
    commandList->SetDescriptorTable((int)eRootBindings::TargetTexture, renderTargets);
    commandList->FlushResourceBarriers();

    GPU::ClearRTV(*commandList.get(), toneMapper->RenderTargets(), toneMapper->GetNumberOfTargets());
    commandList->SetRenderTargets(1, target, nullptr);
    commandList->TrackResource(*target);

    commandList->GetGraphicsCommandList()->IASetVertexBuffers(0, 1, nullptr);
    commandList->GetGraphicsCommandList()->IASetIndexBuffer(nullptr);
    commandList->GetGraphicsCommandList()->DrawInstanced(6, 1, 0, 0);
}

void GraphicsEngine::ImGuiPass()
{
    const auto commandQueue = GPU::GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
    const auto commandList = commandQueue->GetCommandList();

    OPTICK_GPU_CONTEXT(commandList->GetGraphicsCommandList().Get());
    OPTICK_GPU_EVENT("ImGui");

    commandList->SetRenderTargets(1, GPU::GetCurrentBackBuffer(), nullptr);

    ImGui::Render();
    ImGuiIO &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    ID3D12DescriptorHeap *heaps[] = {
        GPU::m_ResourceDescriptors[static_cast<int>(eHeapTypes::HEAP_TYPE_CBV_SRV_UAV)]->Heap(),
        GPU::m_ResourceDescriptors[static_cast<int>(eHeapTypes::HEAP_TYPE_SAMPLER)]->Heap()};
    commandList->GetGraphicsCommandList()->SetDescriptorHeaps((UINT)std::size(heaps), heaps);

    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList->GetGraphicsCommandList().Get());
    commandQueue->ExecuteCommandList(commandList);
}

FORCEINLINE std::shared_ptr<Texture> GraphicsEngine::GetTargetTextures(eRenderTargets type) const
{
    switch (type)
    {
    case eRenderTargets::BackBuffer:
        return std::shared_ptr<Texture>(GPU::GetCurrentBackBuffer());
    case eRenderTargets::DepthBuffer:
        return GPU::m_DepthBuffer;
    case eRenderTargets::SceneBuffer:
        return SceneBuffer;
    case eRenderTargets::halfSceneBuffer:
        return halfSceneBuffer;
    case eRenderTargets::quaterSceneBuffer1:
        return quaterSceneBuffer1;
    case eRenderTargets::quaterSceneBuffer2:
        return quaterSceneBuffer2;
    case eRenderTargets::IntermediateA:
        return IntermediateA;
    case eRenderTargets::IntermediateB:
        return IntermediateB;
    case eRenderTargets::SSAO:
        return SSAOTexture;
    case eRenderTargets::NoiseTexture:
        return NoiseTable->GetRawTexture();
    default:
        return nullptr;
    }
}
