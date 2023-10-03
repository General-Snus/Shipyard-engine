#include "GraphicsEngine.pch.h"
#include "GraphicsEngine.h"
#include "GraphicCommands.h" 
#include <AssetManager/Objects/Components/ComponentDerivatives/LightComponent.h>
#include "Timer.h"
#include <assert.h>
#include <GraphicsEngine/Shaders/Registers.h> 
FrameBuffer& GraphicCommand::GetFrameBuffer()
{
	return GraphicsEngine::Get().myFrameBuffer;
}

ObjectBuffer& GraphicCommand::GetObjectBuffer()
{
	return GraphicsEngine::Get().myObjectBuffer;
}

G_Buffer& GraphicCommand::GetGBuffer()
{
	return GraphicsEngine::Get().myG_Buffer;
}

LineBuffer& GraphicCommand::GetLineBuffer()
{
	return GraphicsEngine::Get().myLineBuffer;
}

LightBuffer& GraphicCommand::GetLightBuffer()
{
	return GraphicsEngine::Get().myLightBuffer;
}

GfxCmd_RenderMesh::GfxCmd_RenderMesh(const RenderData& aData, const Matrix& aTransform)
{
	myElementsData = aData.myMesh->Elements;
	myTransform = aTransform;
	MaxExtents = aData.myMesh->MaxBox;
	MinExtents = aData.myMesh->MinBox;

	for (auto& aMaterial : aData.myMaterials)
	{
		myMaterials.push_back(aMaterial);
	}
}

void GfxCmd_RenderMesh::Execute()
{
	ObjectBuffer& objectBuffer = GetObjectBuffer();
	objectBuffer.Data.myTransform = myTransform;
	objectBuffer.Data.MaxExtents = MaxExtents;
	objectBuffer.Data.MinExtents = MinExtents;
	objectBuffer.Data.hasBone = false;

	RHI::UpdateConstantBufferData(objectBuffer);

	G_Buffer gBuffer = GetGBuffer();
	gBuffer.Data.UseGBufferShader();

	for (auto& aElement : myElementsData)
	{
		if (myMaterials.size() > 0)
		{
			myMaterials[0].lock()->Update();
		}
		RHI::ConfigureInputAssembler(
			aElement.PrimitiveTopology,
			aElement.VertexBuffer,
			aElement.IndexBuffer,
			aElement.Stride,
			Vertex::InputLayout);
		RHI::DrawIndexed(aElement.NumIndices);
	}
}

GfxCmd_SetLightBuffer::GfxCmd_SetLightBuffer()
{
	for (auto& i : GameObjectManager::GetInstance().GetAllComponents<cLight>())
	{
		switch (i.GetType())
		{
		case eLightType::Directional:
		{
			//memcpy(&buff.Data.myDirectionalLight,i.GetData ().get(),sizeof(DirectionalLight));
			DirectionalLight* light = i.GetData<DirectionalLight>().get();
			dirLight.push_back(light);
			break;
		}

		case eLightType::Point:
		{
			//memcpy(&buff.Data.myPointLight[pointLightCount],i.GetData ().get(),sizeof(PointLight));
			PointLight* light = i.GetData<PointLight>().get();
			pointLight.push_back(light);
			break;
		}
		case eLightType::Spot:
		{
			//memcpy(&buff.Data.mySpotLight[spotLightCount],i.GetData().get(),sizeof(SpotLight)); 
			SpotLight* light = i.GetData<SpotLight>().get();
			spotLight.push_back(light);
			break;
		}

		default:
			break;
		}
	}
}

void GfxCmd_SetLightBuffer::Execute()
{
	G_Buffer& gBuffer = GetGBuffer();

	gBuffer.Data.UseEnviromentShader();
	for (auto& light : dirLight)
	{
		LightBuffer& buff = GetLightBuffer();
		buff.Data.myDirectionalLight.Color = light->Color;
		buff.Data.myDirectionalLight.Power = light->Power;
		buff.Data.myDirectionalLight.lightView = light->lightView;
		buff.Data.myDirectionalLight.projection = light->projection;

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



	gBuffer.Data.UsePointlightShader();
	for (auto& light : pointLight)
	{
		LightBuffer& buff = GetLightBuffer();
		buff.Data.myDirectionalLight.Color = light->Color;
		buff.Data.myDirectionalLight.Power = light->Power;
		buff.Data.myPointLight.Color = light->Color;
		buff.Data.myPointLight.Power = light->Power;
		buff.Data.myPointLight.Range = light->Range;
		buff.Data.myPointLight.Position = light->Position;

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

	gBuffer.Data.UseSpotlightShader();
	for (auto& light : spotLight)
	{
		LightBuffer& buff = GetLightBuffer();
		buff.Data.mySpotLight.Color = light->Color;
		buff.Data.mySpotLight.Power = light->Power;
		buff.Data.mySpotLight.Range = light->Range;
		buff.Data.mySpotLight.InnerConeAngle = light->InnerConeAngle;
		buff.Data.mySpotLight.OuterConeAngle = light->OuterConeAngle;
		buff.Data.mySpotLight.Direction = light->Direction;
		buff.Data.mySpotLight.Position = light->Position;

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

GfxCmd_SetFrameBuffer::GfxCmd_SetFrameBuffer(const Matrix& ProjectionMatrix, const Transform& ref, int aRenderMode)
{
	myViewMatrix = Matrix::GetFastInverse(ref.GetTransform());
	myProjectionMatrix = ProjectionMatrix;
	myDeltaTime = static_cast<float>(CommonUtilities::Timer::GetInstance().GetTotalTime());
	myPosition = ref.GetPosition();
	RenderMode = aRenderMode;
}

void GfxCmd_SetFrameBuffer::Execute()
{
	FrameBuffer& buffert = GetFrameBuffer();
	buffert.Data.ProjectionMatrix = myProjectionMatrix;
	buffert.Data.ViewMatrix = myViewMatrix;
	buffert.Data.Time = myDeltaTime;
	buffert.Data.FB_RenderMode = RenderMode;

	buffert.Data.FB_CameraPosition[0] = myPosition.x;
	buffert.Data.FB_CameraPosition[1] = myPosition.y;
	buffert.Data.FB_CameraPosition[2] = myPosition.z;

	RHI::UpdateConstantBufferData(buffert);
}

GfxCmd_RenderSkeletalMesh::GfxCmd_RenderSkeletalMesh(const RenderData& aData,
	const Matrix& aTransform, const Matrix* aBoneTransformList, unsigned int aNumBones) : GfxCmd_RenderMesh(aData, aTransform)
{
	assert(aNumBones < 128);
	for (size_t i = 0; i < 128; i++)
	{
		myBoneTransforms[i] = aBoneTransformList[i];
	}
}

void GfxCmd_RenderSkeletalMesh::Execute()
{
	ObjectBuffer& objectBuffer = GetObjectBuffer();
	objectBuffer.Data.myTransform = myTransform;
	objectBuffer.Data.MaxExtents = MaxExtents;
	objectBuffer.Data.MinExtents = MinExtents;
	objectBuffer.Data.hasBone = true;
	for (int i = 0; i < 128; i++)
	{
		objectBuffer.Data.myBoneTransforms[i] = myBoneTransforms[i];
	}

	RHI::UpdateConstantBufferData(objectBuffer);
	G_Buffer gBuffer = GetGBuffer();
	gBuffer.Data.UseGBufferShader();

	for (auto& aElement : myElementsData)
	{
		myMaterials[0].lock()->Update();
		RHI::ConfigureInputAssembler(aElement.PrimitiveTopology,
			aElement.VertexBuffer,
			aElement.IndexBuffer,
			aElement.Stride,
			Vertex::InputLayout);
		RHI::DrawIndexed(aElement.NumIndices);
	}
}

GfxCmd_RenderSkybox::GfxCmd_RenderSkybox(std::shared_ptr<Texture> texture) : mySkyboxTexture(texture)
{
}

void GfxCmd_RenderSkybox::Execute()
{
	//RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 100	,mySkyboxTexture.get());
}

GfxCmd_DrawDebugPrimitive::GfxCmd_DrawDebugPrimitive(Debug::DebugPrimitive primitive, Matrix Transform) : myPrimitive(primitive), myTransform(Transform)
{

}

void GfxCmd_DrawDebugPrimitive::Execute()
{
	LineBuffer& lineBuffer = GetLineBuffer();
	lineBuffer.Data.myTransform = myTransform;
	RHI::UpdateConstantBufferData(lineBuffer);

	RHI::ConfigureInputAssembler(
		D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		myPrimitive.VertexBuffer,
		myPrimitive.IndexBuffer,
		sizeof(Debug::DebugVertex),
		Debug::DebugVertex::InputLayout);

	RHI::SetVertexShader(GraphicsEngine::Get().GetDebugLineVS().get());
	RHI::SetPixelShader(GraphicsEngine::Get().GetDebugLinePS().get());
	RHI::DrawIndexed(static_cast<UINT>(myPrimitive.NumIndices));
}
