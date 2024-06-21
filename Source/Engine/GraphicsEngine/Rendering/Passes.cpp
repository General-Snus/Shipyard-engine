#include "Engine/GraphicsEngine/GraphicsEngine.pch.h"

#include <Engine/GraphicsEngine/Rendering/Buffers/LightBuffer.h>
#include "Passes.h"

#include "DirectX/Shipyard/CommandList.h"
#include "DirectX/Shipyard/GPU.h"
#include "DirectX/Shipyard/RootSignature.h"
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


void Passes::WriteShadows(std::shared_ptr<CommandList>& commandList, GameObjectManager& scene)
{
	OPTICK_EVENT(); 

	const auto& meshRendererList = scene.GetAllComponents<cMeshRenderer>();
	const auto graphicCommandList = commandList->GetGraphicsCommandList();

	const auto& shadowMapper = PSOCache::GetState(PSOCache::ePipelineStateID::ShadowMapper);

	const auto& pipelineState = shadowMapper->GetPipelineState().Get();
	graphicCommandList->SetPipelineState(pipelineState);
	commandList->TrackResource(pipelineState);

	auto renderShadows = [](const std::vector<cMeshRenderer>& objectsToRender, const std::shared_ptr<CommandList>& list, const std::string_view debugName)
		{
			MaterialBuffer materialBuffer;
			for (const auto& object : objectsToRender)
			{
				if (!object.IsActive()) { continue; }
				const auto& transform = object.GetComponent<Transform>();
				list->AllocateBuffer<ObjectBuffer>(eRootBindings::objectBuffer, { transform.GetRawTransform() });
				for (auto& element : object.GetElements())
				{
					GPU::ConfigureInputAssembler(*list, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, element.IndexResource);
					materialBuffer.vertexBufferIndex = element.VertexBuffer.GetHandle(ViewType::SRV).heapOffset;
					list->AllocateBuffer<MaterialBuffer>(eRootBindings::materialBuffer, materialBuffer);
					OPTICK_GPU_EVENT(debugName.data());
					list->GetGraphicsCommandList()->DrawIndexedInstanced(element.IndexResource.GetIndexCount(), 1, 0, 0, 0);
				}
			}
		};
	auto setShadowPrerequisite = [](const cLight& light, int map, std::shared_ptr<CommandList> const& list) -> std::shared_ptr<Texture>
		{
			const auto& shadowMap = light.GetShadowMap(map);
			list->GetGraphicsCommandList()->RSSetViewports(1, &shadowMap->GetViewPort());
			list->GetGraphicsCommandList()->RSSetScissorRects(1, &shadowMap->GetRect());

			list->TransitionBarrier(*shadowMap, D3D12_RESOURCE_STATE_DEPTH_WRITE);
			list->TrackResource(*shadowMap);
			GPU::ClearDepth(*list, shadowMap.get());
			list->SetRenderTargets(0, nullptr, shadowMap.get());
			list->AllocateBuffer<FrameBuffer>(eRootBindings::frameBuffer, light.GetShadowMapFrameBuffer(map));

			return shadowMap;
		};

	for (auto& light : scene.GetAllComponents<cLight>())
	{
		if (!light.IsActive() || !light.GetIsShadowCaster() || light.GetIsRendered())
		{
			continue;
		}

		switch (light.GetType())
		{
		case eLightType::Directional:
		{
			const std::shared_ptr<Texture>& shadowMap = setShadowPrerequisite(light, 0, commandList);
			renderShadows(meshRendererList, commandList, "DirectionalLight");
			shadowMap->SetView(ViewType::SRV);

			commandList->TransitionBarrier(*shadowMap, D3D12_RESOURCE_STATE_DEPTH_READ | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
			commandList->TrackResource(*shadowMap);
		}
		break;
		case eLightType::Point:
		{
			for (int j = 0; j < 6; j++)
			{
				const std::shared_ptr<Texture>& shadowMap = setShadowPrerequisite(light, j, commandList);
				renderShadows(meshRendererList, commandList, "PointlightShadowDraw");
				shadowMap->SetView(ViewType::SRV);

				commandList->TransitionBarrier(*shadowMap, D3D12_RESOURCE_STATE_DEPTH_READ | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
				commandList->TrackResource(*shadowMap);
			}
		}
		break;
		case eLightType::Spot:
		{
			const std::shared_ptr<Texture>& shadowMap = setShadowPrerequisite(light, 0, commandList);
			renderShadows(meshRendererList, commandList, "SpotlightShadowDraw");
			shadowMap->SetView(ViewType::SRV);

			commandList->TransitionBarrier(*shadowMap, D3D12_RESOURCE_STATE_DEPTH_READ | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
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

LightBuffer Passes::CreateLightBuffer(GameObjectManager& scene)
{
	OPTICK_EVENT();
	LightBuffer lightBuffer;
	lightBuffer.NullifyMemory();

	for (auto& i : scene.GetAllComponents<cLight>())
	{
		switch (i.GetType())
		{
		case eLightType::Directional:
		{
			auto* data = i.GetData<DirectionalLight>().get();
			if (i.GetIsShadowCaster())
			{
				data->shadowMapIndex = i.GetShadowMap(0)->GetHandle(ViewType::SRV).heapOffset;
			}
			auto& directionalLight = lightBuffer.directionalLight;
			directionalLight = *data;
			directionalLight.castShadow = i.GetIsShadowCaster();
			break;
		}
		//REFACTOR
		case eLightType::Point:
		{
			auto* data = i.GetData<PointLight>().get();
			if (i.GetIsShadowCaster())
			{
				for (int j = 0; j < 6; j++)
				{
					data->shadowMapIndex[j] = i.GetShadowMap(j)->GetHandle(ViewType::SRV).heapOffset;
				}
			}
			auto& pointlight = lightBuffer.pointLight[lightBuffer.pointLightAmount];
			pointlight = *data;
			pointlight.castShadow = i.GetIsShadowCaster();
			lightBuffer.pointLightAmount = ((lightBuffer.pointLightAmount + 1) % 8);
			break;
		}
		case eLightType::Spot:
		{
			auto* data = i.GetData<SpotLight>().get();
			if (i.GetIsShadowCaster())
			{
				data->shadowMapIndex = i.GetShadowMap(0)->GetHandle(ViewType::SRV).heapOffset;
			}
			auto& spotLight = lightBuffer.spotLight[lightBuffer.spotLightAmount];
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
