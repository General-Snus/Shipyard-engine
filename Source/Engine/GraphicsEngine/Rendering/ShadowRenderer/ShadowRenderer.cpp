#include "GraphicsEngine.pch.h"
#include <Tools/ImGui/ImGui/imgui.h>
#include <Tools/Optick/src/optick.h>
#include "../../Shaders/Include/Default_VS.h"
#include "ShadowRenderer.h" 

void ShadowRenderer::Init()
{
	ShadowCommandList.Initialize((size_t)10 * MegaByte);
	myVertexShader = GraphicsEngine::Get().myVertexShader;
}

void ShadowRenderer::Execute()
{
	OPTICK_EVENT();
	LightBuffer& buffer = GraphicsEngine::Get().myLightBuffer;

	RHI::SetVertexShader(myVertexShader);
	RHI::SetPixelShader(nullptr);
	std::shared_ptr<Texture> shadowMap;
	for (auto& i : GameObjectManager::Get().GetAllComponents<cLight>())
	{
		if (i.GetIsShadowCaster() && i.GetIsDirty())
		{
			if (i.GetType() == eLightType::Directional && !i.GetIsRendered())
			{
				RHI::BeginEvent(L"DirectionalLight Shadow Pass");
				buffer.Data.myDirectionalLight = *i.GetData<DirectionalLight>().get();
				RHI::SetConstantBuffer(PIPELINE_STAGE_VERTEX_SHADER,REG_LightBuffer,buffer);
				RHI::UpdateConstantBufferData(buffer);
				shadowMap = i.GetShadowMap(0);
				RHI::SetTextureResource(PIPELINE_STAGE_VERTEX_SHADER,REG_dirLightShadowMap,nullptr); // cant be bound to resources when rendering to it
				RHI::SetRenderTarget(nullptr,shadowMap.get());
				RHI::ClearDepthStencil(shadowMap.get());
				GfxCmd_SetFrameBuffer(buffer.Data.myDirectionalLight.projection,buffer.Data.myDirectionalLight.lightView,0).ExecuteAndDestroy();

				ShadowCommandList.StartOver();
				ShadowCommandList.Execute();
				GraphicsEngine::Get().myInstanceRenderer.Execute(true);
				RHI::SetRenderTarget(nullptr,nullptr);
				i.SetIsRendered(true);
				RHI::EndEvent();
			}

			if (i.GetType() == eLightType::Spot && !i.GetIsRendered())
			{
				RHI::BeginEvent(L"SpotLight Shadow Pass");
				buffer.Data.mySpotLight = *i.GetData<SpotLight>().get();
				RHI::SetConstantBuffer(PIPELINE_STAGE_VERTEX_SHADER,REG_LightBuffer,buffer);
				RHI::UpdateConstantBufferData(buffer);

				shadowMap = i.GetShadowMap(0);
				RHI::SetTextureResource(PIPELINE_STAGE_VERTEX_SHADER,REG_dirLightShadowMap,nullptr); // cant be bound to resources when rendering to it
				RHI::SetRenderTarget(nullptr,shadowMap.get());
				RHI::ClearDepthStencil(shadowMap.get());
				GfxCmd_SetFrameBuffer(buffer.Data.mySpotLight.projection,buffer.Data.mySpotLight.lightView,0).ExecuteAndDestroy();

				ShadowCommandList.StartOver();
				ShadowCommandList.Execute();
				GraphicsEngine::Get().myInstanceRenderer.Execute(true);
				RHI::SetRenderTarget(nullptr,nullptr);
				i.SetIsRendered(true);
				RHI::EndEvent();
			}

			if (i.GetType() == eLightType::Point && !i.GetIsRendered())
			{
				RHI::BeginEvent(L"PointLight Shadow Pass");
				for (int j = 0; j < 6; j++)
				{
					buffer.Data.myPointLight = *i.GetData<PointLight>().get();
					RHI::SetConstantBuffer(PIPELINE_STAGE_VERTEX_SHADER,REG_LightBuffer,buffer);
					RHI::UpdateConstantBufferData(buffer);

					shadowMap = i.GetShadowMap(j);
					RHI::SetTextureResource(PIPELINE_STAGE_VERTEX_SHADER,REG_dirLightShadowMap,nullptr); // cant be bound to resources when rendering to it
					RHI::SetRenderTarget(nullptr,shadowMap.get());
					RHI::ClearDepthStencil(shadowMap.get());
					GfxCmd_SetFrameBuffer(buffer.Data.myPointLight.projection,i.GetLightViewMatrix(j),0).ExecuteAndDestroy();

					ShadowCommandList.StartOver();
					ShadowCommandList.Execute();
					GraphicsEngine::Get().myInstanceRenderer.Execute(true);
					RHI::SetRenderTarget(nullptr,nullptr);
					i.SetIsRendered(true);
				}
				RHI::EndEvent();
			}
		}
	}
	ShadowCommandList.ForceSetDone();
}

void ShadowRenderer::ResetShadowList()
{
	ShadowCommandList.ForceSetDone();
	ShadowCommandList.Reset();
}
