#include "GraphicsEngine.pch.h"
#include "GraphicsEngine.h"
#include "GraphicCommands.h" 
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

GfxCmd_SetFrameBuffer::GfxCmd_SetFrameBuffer(const Matrix& aViewMatrix,const Matrix& aProjectionMatrix,Vector3f CameraWorldPosition)
{
	myViewMatrix = aViewMatrix;
	myProjectionMatrix = aProjectionMatrix;
	myDeltaTime = static_cast<float>(CommonUtilities::Timer::GetInstance().GetTotalTime());
	myPosition = CameraWorldPosition;
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
