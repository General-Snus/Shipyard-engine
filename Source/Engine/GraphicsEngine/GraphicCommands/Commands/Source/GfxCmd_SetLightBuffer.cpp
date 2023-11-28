#include <GraphicsEngine.pch.h>
#include "../Headers/GfxCmd_SetLightBuffer.h"
#include <Engine/AssetManager/ComponentSystem/Components/LightComponent.h>

GfxCmd_SetLightBuffer::GfxCmd_SetLightBuffer()
{
	for(auto& i : GameObjectManager::GetInstance().GetAllComponents<cLight>())
	{
		switch(i.GetType())
		{
		case eLightType::Directional:
		{
			std::pair<DirectionalLight*,Texture*> pair;
			pair.first = i.GetData<DirectionalLight>().get();
			if(i.GetIsShadowCaster())
			{
				pair.second = i.GetShadowMap(0).get();
			}

			dirLight.push_back(pair);
			break;
		}

		case eLightType::Point:
		{
			if(i.GetIsShadowCaster())
			{
				for(int j = 0; j < 6; j++)//REFACTOR
				{
					std::pair<PointLight*,Texture*> pair;
					pair.first = i.GetData<PointLight>().get();
					pair.first->lightView = i.GetLightViewMatrix(j);
					pair.second = i.GetShadowMap(j).get();
					pointLight.push_back(pair);
				}
			}
			break;
		}
		case eLightType::Spot:
		{
			std::pair< SpotLight*,Texture*> pair;
			pair.first = i.GetData<SpotLight>().get();

			if(i.GetIsShadowCaster())
			{
				pair.second = i.GetShadowMap(0).get();
			}
			spotLight.push_back(pair);
			break;
		}

		default:
			break;
		}
	}
}

void GfxCmd_SetLightBuffer::ExecuteAndDestroy()
{
	G_Buffer& gBuffer = GetGBuffer(); 
	gBuffer.UseEnviromentShader();

	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,REG_SSAO,GraphicsEngine::Get().GetTargetTextures(eRenderTargets::SSAO).get());
	for(const auto& [light,shadowMap] : dirLight)
	{
		LightBuffer& buff = GetLightBuffer();
		buff.Data.myDirectionalLight = *light;
		RHI::SetTextureResource( PIPELINE_STAGE_PIXEL_SHADER,REG_dirLightShadowMap,shadowMap);

		RHI::UpdateConstantBufferData(buff);
		RHI::ConfigureInputAssembler(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
			nullptr,
			nullptr,
			0,
			nullptr
		);
		RHI::Draw(4);
	}
	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,REG_SSAO,nullptr);

	gBuffer.UsePointlightShader();
	for(const auto& [light,shadowMap] : pointLight)
	{
		LightBuffer& buff = GetLightBuffer();
		buff.Data.myPointLight = *light;  
		RHI::SetTextureResource(  PIPELINE_STAGE_PIXEL_SHADER,REG_dirLightShadowMap,shadowMap);

		RHI::UpdateConstantBufferData(buff);
		RHI::ConfigureInputAssembler(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
			nullptr,
			nullptr,
			0,
			nullptr
		);
		RHI::Draw(4);
	}

	gBuffer.UseSpotlightShader();
	for(const auto& [light,shadowMap] : spotLight)
	{
		LightBuffer& buff = GetLightBuffer();
		buff.Data.mySpotLight = *light;
		if(shadowMap)
		{
			RHI::SetTextureResource(  PIPELINE_STAGE_PIXEL_SHADER,REG_dirLightShadowMap,shadowMap);
		}

		RHI::UpdateConstantBufferData(buff);
		RHI::ConfigureInputAssembler(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
			nullptr,
			nullptr,
			0,
			nullptr
		);
		RHI::Draw(4);
	}
	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,REG_dirLightShadowMap,nullptr);
}