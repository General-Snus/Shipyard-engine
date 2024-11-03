#include "GraphicsEngine.pch.h"

#include "Passes.h"
#include <Engine/GraphicsEngine/Rendering/Buffers/LightBuffer.h>

#include "DirectX/DX12/Graphics/CommandList.h"
#include "DirectX/DX12/Graphics/GPU.h"
#include "DirectX/DX12/Graphics/PSO.h"
#include "DirectX/DX12/Graphics/RootSignature.h"

#include "Editor/Editor/Core/Editor.h"
#include "Editor/Editor/Windows/Window.h"
#include "Engine/AssetManager/AssetManager.h"

#include "Engine/AssetManager/ComponentSystem/Components/LightComponent.h"
#include "Engine/AssetManager/ComponentSystem/Components/MeshRenderer.h"
#include "Engine/AssetManager/ComponentSystem/Components/Transform.h"
#include "Engine/AssetManager/ComponentSystem/GameObjectManager.h"
#include "Engine/AssetManager/Objects/BaseAssets/MaterialAsset.h"
#include "Engine/GraphicsEngine/Rendering/Buffers/FrameBuffer.h"
#include "Engine/GraphicsEngine/Rendering/Buffers/ObjectBuffer.h"

namespace Passes
{
void WriteShadows(const GraphicsEngine &instance, std::shared_ptr<CommandList> &commandList, GameObjectManager &scene)
{
	OPTICK_GPU_EVENT("WriteShadows");

	const auto &meshRendererList = scene.GetAllComponents<MeshRenderer>();
	const auto graphicCommandList = commandList->GetGraphicsCommandList();

	const auto &shadowMapper = instance.GetPSOCache().GetState(PSOCache::ePipelineStateID::ShadowMapper);

	const auto &pipelineState = shadowMapper->GetPipelineState().Get();
	graphicCommandList->SetPipelineState(pipelineState);
	commandList->TrackResource(pipelineState);

	auto renderShadows = [](const std::vector<MeshRenderer> &objectsToRender, const std::shared_ptr<CommandList> &list,
							const std::string_view debugName) {
		debugName;
		MaterialBuffer materialBuffer;
		for (const auto &object : objectsToRender)
		{
			if (!object.IsActive())
			{
				continue;
			}
			const auto &transform = object.GetComponent<Transform>();
			list->AllocateBuffer<ObjectBuffer>(eRootBindings::objectBuffer, {transform.WorldMatrix()});
			for (auto &element : object.GetElements())
			{
				OPTICK_GPU_EVENT(debugName.data());
				GPUInstance.ConfigureInputAssembler(*list, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, element.IndexResource);
				materialBuffer.vertexBufferIndex = element.VertexBuffer.GetHandle(ViewType::SRV).heapOffset;
				list->AllocateBuffer<MaterialBuffer>(eRootBindings::materialBuffer, materialBuffer);
				list->GetGraphicsCommandList()->DrawIndexedInstanced(element.IndexResource.GetIndexCount(), 1, 0, 0, 0);
			}
		}
	};
	auto setShadowPrerequisite = [](const Light &light, int map,
									std::shared_ptr<CommandList> const &list) -> std::shared_ptr<Texture> {
		OPTICK_GPU_EVENT("setShadowPrerequisite");
		const auto &shadowMap = light.GetShadowMap(map);
		if (!shadowMap)
		{
			return nullptr;
		}

		list->GetGraphicsCommandList()->RSSetViewports(1, &shadowMap->GetViewPort());
		list->GetGraphicsCommandList()->RSSetScissorRects(1, &shadowMap->GetRect());

		list->TransitionBarrier(*shadowMap, D3D12_RESOURCE_STATE_DEPTH_WRITE);
		list->TrackResource(*shadowMap);
		GPUInstance.ClearDepth(*list, shadowMap.get());
		list->SetRenderTargets(0, nullptr, shadowMap.get());
		list->AllocateBuffer<FrameBuffer>(eRootBindings::frameBuffer, light.GetShadowMapFrameBuffer(map));

		return shadowMap;
	};

	for (auto &light : scene.GetAllComponents<Light>())
	{
		if (!light.IsActive() || !light.GetIsShadowCaster() || light.GetIsRendered())
		{
			continue;
		}

		switch (light.GetType())
		{
		case eLightType::Directional: {
			OPTICK_GPU_EVENT("DirectionalShadow");
			const std::shared_ptr<Texture> &shadowMap = setShadowPrerequisite(light, 0, commandList);
			if (!shadowMap)
			{
				continue;
			}

			renderShadows(meshRendererList, commandList, "DirectionalLight");
			shadowMap->SetView(ViewType::SRV);

			commandList->TransitionBarrier(*shadowMap, D3D12_RESOURCE_STATE_DEPTH_READ |
														   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
			commandList->TrackResource(*shadowMap);
		}
		break;
		case eLightType::Point: {
			OPTICK_GPU_EVENT("PointlightShadow");
			for (int j = 0; j < 6; j++)
			{
				const std::shared_ptr<Texture> &shadowMap = setShadowPrerequisite(light, j, commandList);
				if (!shadowMap)
				{
					continue;
				}

				renderShadows(meshRendererList, commandList, "PointlightShadowDraw");
				shadowMap->SetView(ViewType::SRV);

				commandList->TransitionBarrier(*shadowMap, D3D12_RESOURCE_STATE_DEPTH_READ |
															   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
				commandList->TrackResource(*shadowMap);
			}
		}
		break;
		case eLightType::Spot: {
			OPTICK_GPU_EVENT("SpotlightShadow");
			const std::shared_ptr<Texture> &shadowMap = setShadowPrerequisite(light, 0, commandList);
			if (!shadowMap)
			{
				continue;
			}

			renderShadows(meshRendererList, commandList, "SpotlightShadowDraw");
			shadowMap->SetView(ViewType::SRV);

			commandList->TransitionBarrier(*shadowMap, D3D12_RESOURCE_STATE_DEPTH_READ |
														   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
			commandList->TrackResource(*shadowMap);
		}
		break;
		case eLightType::uninitialized:
			std::unreachable();
			break;
		default:
			std::unreachable();
			break;
		}
		light.SetIsRendered(true);
	}
}

LightBuffer CreateLightBuffer(GameObjectManager &scene)
{
	LightBuffer lightBuffer;
	lightBuffer.NullifyMemory();

	for (auto &i : scene.GetAllComponents<Light>())
	{
		if (!i.IsActive())
		{
			continue;
		}

		switch (i.GetType())
		{
		case eLightType::Directional: {
			OPTICK_EVENT("Directional");
			auto *data = i.GetData<DirectionalLight>().get();
			if (i.GetIsShadowCaster())
			{
				data->shadowMapIndex = i.GetShadowMap(0)->GetHandle(ViewType::SRV).heapOffset;
			}
			auto &directionalLight = lightBuffer.directionalLight;
			directionalLight = *data;
			directionalLight.castShadow = i.GetIsShadowCaster();
			break;
		}
		// REFACTOR
		case eLightType::Point: {
			OPTICK_EVENT("Point");
			auto *data = i.GetData<PointLight>().get();
			if (i.GetIsShadowCaster())
			{
				for (int j = 0; j < 6; j++)
				{
					data->shadowMapIndex[j] = i.GetShadowMap(j)->GetHandle(ViewType::SRV).heapOffset;
				}
			}
			auto &pointlight = lightBuffer.pointLight[lightBuffer.pointLightAmount];
			pointlight = *data;
			pointlight.castShadow = i.GetIsShadowCaster();
			lightBuffer.pointLightAmount = ((lightBuffer.pointLightAmount + 1) % 8);
			break;
		}
		case eLightType::Spot: {
			OPTICK_EVENT("Spot");
			auto *data = i.GetData<SpotLight>().get();
			if (i.GetIsShadowCaster())
			{
				data->shadowMapIndex = i.GetShadowMap(0)->GetHandle(ViewType::SRV).heapOffset;
			}
			auto &spotLight = lightBuffer.spotLight[lightBuffer.spotLightAmount];
			spotLight = *data;
			spotLight.castShadow = i.GetIsShadowCaster();
			lightBuffer.spotLightAmount = ((lightBuffer.spotLightAmount + 1) % 8);
			break;
		}

		default:
			break;
		}
	}
	return lightBuffer;
}
} // namespace Passes

void SSAO::Initialize(void)
{
}
  
void SSAO::Render(const GraphicsEngine &instance, std::shared_ptr<CommandList> &commandList, const Camera &camera)
{
	instance;
	commandList;
	camera;
}

void GBuffer::Initialize(const GraphicsEngine &instance)
{
	UNREFERENCED_PARAMETER(instance);
}

void GBuffer::Render(const GraphicsEngine &instance, std::shared_ptr<CommandList> &commandList,
					 const GameObjectManager &scene)
{ 
	commandList->SetViewports(GPUInstance.m_Viewport);
	commandList->SetSissorRect(GPUInstance.m_ScissorRect);
	 
	const auto & gbufferPSO = instance.m_Cache->GetState(PSOCache::ePipelineStateID::GBuffer); 
	const auto &gBufferTextures = gbufferPSO->RenderTargets(); 
	const auto numTargets = gbufferPSO->GetNumberOfTargets();

	commandList->ClearRenderTargets(numTargets, gBufferTextures);
	commandList->SetRenderTargets(numTargets, gBufferTextures,
								  GPUInstance.m_DepthBuffer.get());

	commandList->SetPipelineState(*gbufferPSO);  

	std::vector<MeshRenderer> list;
	if (!scene.GetReadOnly<MeshRenderer>(list))
	{
		return;
	}

	const uint32_t dtAlbedo = instance.GetDefaultTexture(eTextureType::ColorMap)->GetRawTexture()->GetHeapOffset();
	const uint32_t dtNormal = instance.GetDefaultTexture(eTextureType::NormalMap)->GetRawTexture()->GetHeapOffset();
	const uint32_t dtMaterial = instance.GetDefaultTexture(eTextureType::MaterialMap)->GetRawTexture()->GetHeapOffset();
	const uint32_t dtEffect = instance.GetDefaultTexture(eTextureType::EffectMap)->GetRawTexture()->GetHeapOffset();

	int vertCount = 0;
	for (const auto &meshRenderer : list)
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
			GPUInstance.ConfigureInputAssembler(*commandList, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
												element.IndexResource);

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
						Logger.Critical("Texture type is not found or is not valid for used on deffered");
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
						Logger.Critical("Texture type is not found or is not valid for used on deffered");
						break;
					}
				}
			}
			materialBuffer.vertexBufferIndex = element.VertexBuffer.GetHandle(ViewType::SRV).heapOffset;
			materialBuffer.vertexOffset = 0;
			// vertex offset is part of drawcall, if i ever use this i need to set it here

			assert(materialBuffer.albedoTexture != -1 && "HEAP INDEX OUT OF BOUND");
			assert(materialBuffer.normalTexture != -1 && "HEAP INDEX OUT OF BOUND");
			assert(materialBuffer.materialTexture != -1 && "HEAP INDEX OUT OF BOUND");
			assert(materialBuffer.emissiveTexture != -1 && "HEAP INDEX OUT OF BOUND");
			assert(materialBuffer.vertexBufferIndex != -1 && "HEAP INDEX OUT OF BOUND");
			assert(materialBuffer.vertexOffset != -1 && "HEAP INDEX OUT OF BOUND");

			const auto &alloc2 = GPUInstance.m_GraphicsMemory->AllocateConstant<MaterialBuffer>(materialBuffer);
			commandList->GetGraphicsCommandList()->SetGraphicsRootConstantBufferView(REG_DefaultMaterialBuffer,
																					 alloc2.GpuAddress());

			OPTICK_GPU_EVENT("Draw");
			// TODO Circular index buffer for pre allocs one day -_-
			commandList->GetGraphicsCommandList()->DrawIndexedInstanced(element.IndexResource.GetIndexCount(), 1, 0, 0,
																		0);
		}
	}
}
