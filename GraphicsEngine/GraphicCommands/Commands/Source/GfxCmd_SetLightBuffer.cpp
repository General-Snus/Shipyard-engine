#include <GraphicsEngine.pch.h>
#include "../Headers/GfxCmd_SetLightBuffer.h"
#include <AssetManager/Objects/Components/ComponentDerivatives/LightComponent.h>

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
				//for(int j = 0; j < 6; j++)//REFACTOR
				//{
					std::pair< PointLight*,Texture*> pair;
					pair.first =  i.GetData<PointLight>().get();
					//pair.first->lightView = i.GetLightViewMatrix(j);
					pair.second = i.GetShadowMap(0).get();
					pointLight.push_back(pair);
				//}
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
	for(auto& light : dirLight)
	{
		LightBuffer& buff = GetLightBuffer();
		buff.Data.myDirectionalLight = *light.first;
		RHI::SetTextureResource(PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER,REG_dirLightShadowMap,light.second);

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

	gBuffer.UsePointlightShader();
	for(auto& light : pointLight)
	{
		LightBuffer& buff = GetLightBuffer();
		buff.Data.myPointLight = *light.first;
		RHI::SetTextureResource(PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER,REG_dirLightShadowMap,light.second);

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
	 RHI::SetGeometryShader(nullptr);

	gBuffer.UseSpotlightShader();
	for(auto& light : spotLight)
	{
		LightBuffer& buff = GetLightBuffer();
		buff.Data.mySpotLight = *light.first;
		RHI::SetTextureResource(PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER,REG_dirLightShadowMap,light.second);

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
}