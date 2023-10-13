#include "GraphicsEngine.pch.h"
#include "ShadowRenderer.h"
#include "../../Shaders/Include/PointLightShadow_VS.h"
#include "../../Shaders/Include/PointLightShadow_GS.h"
#include "../../Shaders/Include/PointLightShadow_PS.h"

void ShadowRenderer::Init()
{
	ShadowCommandList.Initialize();
	RHI::CreateVertexShader(
		myVertexShader ,
		BuiltIn_PointLightShadow_VS_ByteCode,
		sizeof(BuiltIn_PointLightShadow_VS_ByteCode)
	); 
		RHI::CreateGeometryShader(
			myGeoShader,
			BuiltIn_PointLightShadow_GS_ByteCode,
			sizeof(BuiltIn_PointLightShadow_GS_ByteCode)
		);
	RHI::CreatePixelShader(
		myPixelShader,
		BuiltIn_PointLightShadow_PS_ByteCode,
		sizeof(BuiltIn_PointLightShadow_PS_ByteCode)
	);
}

void ShadowRenderer::Execute()
{
	LightBuffer& buffer = GraphicsEngine::Get().myLightBuffer;
	RHI::SetVertexShader(myVertexShader);
	
	RHI::SetPixelShader(myPixelShader);


	std::shared_ptr<Texture> shadowMap;
	for(auto& i : GameObjectManager::GetInstance().GetAllComponents<cLight>())
	{
		if(i.GetIsShadowCaster() && !i.GetIsDirty())
		{
			if(i.GetType() == eLightType::Directional)
			{
				if(!i.GetIsRendered())
				{
					buffer.Data.myDirectionalLight = *i.GetData<DirectionalLight>().get();
					RHI::UpdateConstantBufferData(buffer);
					shadowMap = i.GetShadowMap(0);
					RHI::SetTextureResource(PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER,REG_dirLightShadowMap,nullptr); // cant be bound to resources when rendering to it
					RHI::ClearDepthStencil(shadowMap.get());
					RHI::SetRenderTarget(nullptr,shadowMap.get());
					GfxCmd_SetFrameBuffer(buffer.Data.myDirectionalLight.projection,buffer.Data.myDirectionalLight.lightView,0).ExecuteAndDestroy();
					ShadowCommandList.StartOver();
					ShadowCommandList.Execute();
 
					RHI::SetRenderTarget(nullptr,nullptr);
					i.SetIsRendered(true);
				}
			}

			if(i.GetType() == eLightType::Spot)
			{
				if(!i.GetIsRendered())
				{
					buffer.Data.mySpotLight = *i.GetData<SpotLight>().get();
					RHI::UpdateConstantBufferData(buffer);

					shadowMap = i.GetShadowMap(0);
					RHI::SetTextureResource(PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER,REG_dirLightShadowMap,nullptr); // cant be bound to resources when rendering to it
					RHI::ClearDepthStencil(shadowMap.get());
					RHI::SetRenderTarget(nullptr,shadowMap.get());
					GfxCmd_SetFrameBuffer(buffer.Data.mySpotLight.projection,buffer.Data.mySpotLight.lightView,0).ExecuteAndDestroy();

					ShadowCommandList.StartOver();
					ShadowCommandList.Execute();

					RHI::SetRenderTarget(nullptr,nullptr);
					i.SetIsRendered(true);
				}
			}

			if(i.GetType() == eLightType::Point)
			{
				RHI::SetGeometryShader(myGeoShader);
				if(!i.GetIsRendered())
				{
					buffer.Data.myPointLight = *i.GetData<PointLight>().get();
					RHI::UpdateConstantBufferData(buffer);
					{
						shadowMap = i.GetShadowMap(0);
						RHI::SetTextureResource(PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER,REG_dirLightShadowMap,nullptr); // cant be bound to resources when rendering to it
						RHI::ClearDepthStencil(shadowMap.get());
						RHI::SetRenderTarget(nullptr,shadowMap.get());
						GfxCmd_SetFrameBuffer(buffer.Data.myPointLight.projection,buffer.Data.myPointLight.lightView,0).ExecuteAndDestroy();

						ShadowCommandList.StartOver();
						ShadowCommandList.Execute();

						RHI::SetRenderTarget(nullptr,nullptr);
						i.SetIsRendered(true);
					}
				}
				RHI::SetGeometryShader(nullptr);
			}
		}
	}
	RHI::SetVertexShader(nullptr);
	RHI::SetGeometryShader(nullptr);
	RHI::SetPixelShader(nullptr);
}

void ShadowRenderer::ResetShadowList()
{
	ShadowCommandList.Reset();
}
