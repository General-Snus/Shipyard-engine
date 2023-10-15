#include "GraphicsEngine.pch.h"
#include "ShadowRenderer.h"
#include "../../Shaders/Include/Default_VS.h"
#include <ImGui/imgui.h>

void ShadowRenderer::Init()
{
	ShadowCommandList.Initialize((size_t) 5 * MegaByte); 
}

void ShadowRenderer::Execute()
{
	LightBuffer& buffer = GraphicsEngine::Get().myLightBuffer;  

	std::shared_ptr<Texture> shadowMap;
	for(auto& i : GameObjectManager::GetInstance().GetAllComponents<cLight>())
	{
		if(i.GetIsShadowCaster() && !i.GetIsDirty())
		{
			if(i.GetType() == eLightType::Directional && !i.GetIsRendered())
			{
				buffer.Data.myDirectionalLight = *i.GetData<DirectionalLight>().get();
				RHI::UpdateConstantBufferData(buffer);
				shadowMap = i.GetShadowMap(0);
				RHI::SetTextureResource(PIPELINE_STAGE_VERTEX_SHADER,REG_dirLightShadowMap,nullptr); // cant be bound to resources when rendering to it
				RHI::ClearDepthStencil(shadowMap.get());
				RHI::SetRenderTarget(nullptr,shadowMap.get());
				GfxCmd_SetFrameBuffer(buffer.Data.myDirectionalLight.projection,buffer.Data.myDirectionalLight.lightView,0).ExecuteAndDestroy();

				ShadowCommandList.StartOver();
				ShadowCommandList.Execute();

				RHI::SetRenderTarget(nullptr,nullptr);
				 i.SetIsRendered(true); 
			}

			if(i.GetType() == eLightType::Spot && !i.GetIsRendered())
			{
				buffer.Data.mySpotLight = *i.GetData<SpotLight>().get();
				RHI::UpdateConstantBufferData(buffer);

				shadowMap = i.GetShadowMap(0);
				RHI::SetTextureResource(PIPELINE_STAGE_VERTEX_SHADER,REG_dirLightShadowMap,nullptr); // cant be bound to resources when rendering to it
				RHI::ClearDepthStencil(shadowMap.get());
				RHI::SetRenderTarget(nullptr,shadowMap.get());
				GfxCmd_SetFrameBuffer(buffer.Data.mySpotLight.projection,buffer.Data.mySpotLight.lightView,0).ExecuteAndDestroy();

				ShadowCommandList.StartOver();
				ShadowCommandList.Execute();

				RHI::SetRenderTarget(nullptr,nullptr);
				i.SetIsRendered(true);     
			}

			if(i.GetType() == eLightType::Point && !i.GetIsRendered())
			{
				for(int j = 0; j < 6; j++)
				{
					buffer.Data.myPointLight = *i.GetData<PointLight>().get(); 
					RHI::UpdateConstantBufferData(buffer);

					shadowMap = i.GetShadowMap(j);
					RHI::SetTextureResource(PIPELINE_STAGE_VERTEX_SHADER,REG_dirLightShadowMap,nullptr); // cant be bound to resources when rendering to it
					RHI::ClearDepthStencil(shadowMap.get());
					RHI::SetRenderTarget(nullptr,shadowMap.get());
					GfxCmd_SetFrameBuffer(buffer.Data.myPointLight.projection,i.GetLightViewMatrix(j),0).ExecuteAndDestroy();

					ShadowCommandList.StartOver();
					ShadowCommandList.Execute();
					RHI::SetRenderTarget(nullptr,nullptr);
					i.SetIsRendered(true); 
				}
			}
		}
	} 
}

void ShadowRenderer::ResetShadowList()
{
	ShadowCommandList.Reset();
}
