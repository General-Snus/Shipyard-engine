#include "GraphicsEngine.pch.h"
#include "GraphicsEngine.h"
#include "GraphicCommands.h" 
#include <AssetManager/Objects/Components/ComponentDerivatives/LightComponent.h>
#include "Timer.h"
#include <assert.h>

FrameBuffer& GraphicCommand::GetFrameBuffer()
{
	return GraphicsEngine::Get().myFrameBuffer;
}

ObjectBuffer& GraphicCommand::GetObjectBuffer()
{
	return GraphicsEngine::Get().myObjectBuffer;
}

LightBuffer& GraphicCommand::GetLightBuffer()
{
	return GraphicsEngine::Get().myLightBuffer;
}

GfxCmd_RenderMesh::GfxCmd_RenderMesh(const RenderData& aData,const Matrix& aTransform)
{
	myElementsData = aData.myMesh->Elements;
	myTransform = aTransform;
	MaxExtents = aData.myMesh->MaxBox;
	MinExtents = aData.myMesh->MinBox;

	for(auto& aMaterial : aData.myMaterials)
	{
		myMaterials.push_back(aMaterial);
	}
}
GfxCmd_SetLightBuffer::GfxCmd_SetLightBuffer()
{
	int pointLightCount = 0;
	int spotLightCount = 0; 
	LightBuffer& buff = GetLightBuffer();
	for(auto& i : GameObjectManager::GetInstance().GetAllComponents<cLight>())
	{
		Transform* transform = i.TryGetComponent<Transform>();
		if(transform != nullptr)
		{
			switch(i.GetType())
			{
			case eLightType::Directional:
			{
				//memcpy(&buff.Data.myDirectionalLight,i.GetData ().get(),sizeof(DirectionalLight));
				DirectionalLight* light = i.GetData<DirectionalLight>().get();
				buff.Data.myDirectionalLight.Color = light->Color;
				buff.Data.myDirectionalLight.Intensity = light->Intensity; 
				buff.Data.myDirectionalLight.Direction = light->Direction;
				break;
			}

			case eLightType::Point:
			{
				//memcpy(&buff.Data.myPointLight[pointLightCount],i.GetData ().get(),sizeof(PointLight));
				PointLight* light = i.GetData<PointLight>().get();
				buff.Data.myPointLight[pointLightCount].Color = light->Color;
				buff.Data.myPointLight[pointLightCount].Intensity = light->Intensity; 
				buff.Data.myPointLight[pointLightCount].Range = light->Range;
				buff.Data.myPointLight[pointLightCount].Position = transform->GetPosition();
				pointLightCount++;
				break;
			}
			case eLightType::Spot:
			{
				//memcpy(&buff.Data.mySpotLight[spotLightCount],i.GetData().get(),sizeof(SpotLight)); 
				SpotLight* light = i.GetData<SpotLight>().get();
				buff.Data.mySpotLight[spotLightCount].Color = light->Color;
				buff.Data.mySpotLight[spotLightCount].Intensity = light->Intensity; 
				buff.Data.mySpotLight[spotLightCount].Range = light->Range;
				buff.Data.mySpotLight[spotLightCount].InnerConeAngle = light->InnerConeAngle;
				buff.Data.mySpotLight[spotLightCount].OuterConeAngle = light->OuterConeAngle;
				buff.Data.mySpotLight[spotLightCount].Direction = light->Direction;
				buff.Data.mySpotLight[spotLightCount].Position = transform->GetPosition();
				spotLightCount++;
				break;
			}

			default:
				break;
			}
			continue;
		}

		switch(i.GetType())
		{
		case eLightType::Directional:
		{
			//memcpy(&buff.Data.myDirectionalLight,i.GetData().get(),sizeof(DirectionalLight));
			DirectionalLight* light = i.GetData<DirectionalLight>().get();
			buff.Data.myDirectionalLight.Color = light->Color;
			buff.Data.myDirectionalLight.Intensity = light->Intensity; 
			buff.Data.myDirectionalLight.Direction = light->Direction;
			break;
		}

		case eLightType::Point:
		{
			//memcpy(&buff.Data.myPointLight[pointLightCount],i.GetData().get(),sizeof(PointLight));
			PointLight* light = i.GetData<PointLight>().get();
			buff.Data.myPointLight[pointLightCount].Color = light->Color;
			buff.Data.myPointLight[pointLightCount].Intensity = light->Intensity; 
			buff.Data.myPointLight[pointLightCount].Range = light->Range;
			buff.Data.myPointLight[pointLightCount].Position = light->Position;
			pointLightCount++;
			break;
		}
		case eLightType::Spot:
		{
			//memcpy(&buff.Data.mySpotLight[spotLightCount],i.GetData().get(),sizeof(SpotLight));
			SpotLight* light = i.GetData<SpotLight>().get();
			buff.Data.mySpotLight[spotLightCount].Color = light->Color;
			buff.Data.mySpotLight[spotLightCount].Intensity = light->Intensity; 
			buff.Data.mySpotLight[spotLightCount].Range = light->Range;
			buff.Data.mySpotLight[spotLightCount].InnerConeAngle = light->InnerConeAngle;
			buff.Data.mySpotLight[spotLightCount].OuterConeAngle = light->OuterConeAngle;
			buff.Data.mySpotLight[spotLightCount].Direction = light->Direction;
			buff.Data.mySpotLight[spotLightCount].Position = light->Position;
			spotLightCount++;
			break;
		}

		default:
			break;
		}
	}
}
void GfxCmd_SetLightBuffer::Execute()
{
	RHI::UpdateConstantBufferData(GetLightBuffer());
}
void GfxCmd_RenderMesh::Execute()
{
	ObjectBuffer& objectBuffer = GetObjectBuffer();
	objectBuffer.Data.myTransform = myTransform;
	objectBuffer.Data.MaxExtents = MaxExtents;
	objectBuffer.Data.MinExtents = MinExtents;
	objectBuffer.Data.hasBone = false;

	RHI::UpdateConstantBufferData(objectBuffer);


	for(auto& aElement : myElementsData)
	{
		if(myMaterials.size() > 0)
		{
			myMaterials[0].lock()->Update();

		}
		RHI::ConfigureInputAssembler(aElement.PrimitiveTopology,
			aElement.VertexBuffer,
			aElement.IndexBuffer,
			aElement.Stride,
			Vertex::InputLayout);
		RHI::DrawIndexed(aElement.NumIndices);
	}
}

GfxCmd_SetFrameBuffer::GfxCmd_SetFrameBuffer(const Matrix& ProjectionMatrix,const Transform& ref)
{
	myViewMatrix = Matrix::GetFastInverse(ref.GetTransform());
	myProjectionMatrix = ProjectionMatrix;
	myDeltaTime = static_cast<float>(CommonUtilities::Timer::GetInstance().GetTotalTime());
	myPosition = ref.GetPosition();
}

void GfxCmd_SetFrameBuffer::Execute()
{
	FrameBuffer& buffert = GetFrameBuffer();
	buffert.Data.ProjectionMatrix = myProjectionMatrix;
	buffert.Data.ViewMatrix = myViewMatrix;
	buffert.Data.Time = myDeltaTime;

	buffert.Data.FB_CameraPosition[0] = myPosition.x;
	buffert.Data.FB_CameraPosition[1] = myPosition.y;
	buffert.Data.FB_CameraPosition[2] = myPosition.z;

	RHI::UpdateConstantBufferData(buffert);
}

GfxCmd_RenderSkeletalMesh::GfxCmd_RenderSkeletalMesh(const RenderData& aData,
	const Matrix& aTransform,const Matrix* aBoneTransformList,unsigned int aNumBones) : GfxCmd_RenderMesh(aData,aTransform)
{
	assert(aNumBones < 128);
	for(size_t i = 0; i < 128; i++)
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
	for(int i = 0; i < 128; i++)
	{
		objectBuffer.Data.myBoneTransforms[i] = myBoneTransforms[i];
	}

	RHI::UpdateConstantBufferData(objectBuffer);

	for(auto& aElement : myElementsData)
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
