#include "GraphicsEngine.pch.h"
#include "GraphicsEngine.h"
#include "GraphicCommands.h" 
#include <Modelviewer/Core/Modelviewer.h>
#include <AssetManager/Objects/Components/ComponentDerivatives/LightComponent.h>
#include "Timer.h"
#include <assert.h>
#include <ImGui/imgui.h>
#include <GraphicsEngine/Shaders/Registers.h> 

FrameBuffer& GraphicCommandBase::GetFrameBuffer()
{
	return GraphicsEngine::Get().myFrameBuffer;
}
ObjectBuffer& GraphicCommandBase::GetObjectBuffer()
{
	return GraphicsEngine::Get().myObjectBuffer;
}
G_Buffer& GraphicCommandBase::GetGBuffer()
{
	return GraphicsEngine::Get().myG_Buffer;
}
LineBuffer& GraphicCommandBase::GetLineBuffer()
{
	return GraphicsEngine::Get().myLineBuffer;
}
LightBuffer& GraphicCommandBase::GetLightBuffer()
{
	return GraphicsEngine::Get().myLightBuffer;
}

GfxCmd_SetLightBuffer::GfxCmd_SetLightBuffer()
{
	for (auto& i : GameObjectManager::GetInstance().GetAllComponents<cLight>())
	{
		switch (i.GetType())
		{
		case eLightType::Directional:
		{
			std::pair< DirectionalLight*, Texture*> pair;
			pair.first = i.GetData<DirectionalLight>().get();
			if (i.GetIsShadowCaster())
			{
				pair.second = i.GetShadowMap(0).get();
			}

			dirLight.push_back(pair);
			break;
		}

		case eLightType::Point:
		{
			if (i.GetIsShadowCaster())
			{
				for (int j = 0; j < 6; j++)//REFACTOR
				{
					std::pair< PointLight, Texture*> pair;
					pair.first = *i.GetData<PointLight>().get();
					pair.first.lightView = i.GetLightViewMatrix(j);
					pair.second = i.GetShadowMap(j).get();
					pointLight.push_back(pair);
				}
			}
			break;
		}
		case eLightType::Spot:
		{
			std::pair< SpotLight*, Texture*> pair;
			pair.first = i.GetData<SpotLight>().get();

			if (i.GetIsShadowCaster())
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
	for (auto& light : dirLight)
	{
		LightBuffer& buff = GetLightBuffer();
		buff.Data.myDirectionalLight = *light.first;
		RHI::SetTextureResource(PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER, REG_dirLightShadowMap, light.second);

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
	for (auto& light : pointLight)
	{
		LightBuffer& buff = GetLightBuffer();
		buff.Data.myPointLight = light.first;
		RHI::SetTextureResource(PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER, REG_dirLightShadowMap, light.second);

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
	for (auto& light : spotLight)
	{
		LightBuffer& buff = GetLightBuffer();
		buff.Data.mySpotLight = *light.first;
		RHI::SetTextureResource(PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER, REG_dirLightShadowMap, light.second);

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

GfxCmd_DebugLayer::GfxCmd_DebugLayer()
{

}

void GfxCmd_DebugLayer::ExecuteAndDestroy()
{
	if (ModelViewer::GetApplicationState().filter != DebugFilter::NoFilter)
	{
		G_Buffer& gBuffer = GetGBuffer();
		gBuffer.UseDebugShader();
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

//Remember to invert the matrix
GfxCmd_SetFrameBuffer::GfxCmd_SetFrameBuffer(const Matrix& ProjectionMatrix, const Matrix& ref, int aRenderMode)
{
	myViewMatrix = ref;
	myProjectionMatrix = ProjectionMatrix;
	myDeltaTime = static_cast<float>(CommonUtilities::Timer::GetInstance().GetTotalTime());
	myPosition = { ref(4,1),ref(4,2),ref(4,3) };
	RenderMode = aRenderMode;
}
void GfxCmd_SetFrameBuffer::ExecuteAndDestroy()
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
void GfxCmd_RenderMesh::ExecuteAndDestroy()
{
	ObjectBuffer& objectBuffer = GetObjectBuffer();
	objectBuffer.Data.myTransform = myTransform;
	objectBuffer.Data.MaxExtents = MaxExtents;
	objectBuffer.Data.MinExtents = MinExtents;
	objectBuffer.Data.hasBone = false;

	RHI::UpdateConstantBufferData(objectBuffer);

	G_Buffer gBuffer = GetGBuffer();
	gBuffer.UseGBufferShader();

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

GfxCmd_RenderMeshShadow::GfxCmd_RenderMeshShadow(const RenderData& aMesh, const Matrix& aTransform) : GfxCmd_RenderMesh(aMesh, aTransform)
{
}

void GfxCmd_RenderMeshShadow::ExecuteAndDestroy()
{
	ObjectBuffer& objectBuffer = GetObjectBuffer();
	objectBuffer.Data.myTransform = myTransform;
	objectBuffer.Data.MaxExtents = MaxExtents;
	objectBuffer.Data.MinExtents = MinExtents;
	objectBuffer.Data.hasBone = false;

	RHI::UpdateConstantBufferData(objectBuffer);
	RHI::Context->PSSetShader(nullptr, nullptr, 0);

	for (auto& aElement : myElementsData)
	{
		RHI::ConfigureInputAssembler(
			aElement.PrimitiveTopology,
			aElement.VertexBuffer,
			aElement.IndexBuffer,
			aElement.Stride,
			Vertex::InputLayout);
		RHI::DrawIndexed(aElement.NumIndices);
	}
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
void GfxCmd_RenderSkeletalMesh::ExecuteAndDestroy()
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
	gBuffer.UseGBufferShader();

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
void GfxCmd_RenderSkybox::ExecuteAndDestroy()
{
	//RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, 100	,mySkyboxTexture.get());
}

GfxCmd_DrawDebugPrimitive::GfxCmd_DrawDebugPrimitive(Debug::DebugPrimitive primitive, Matrix Transform) : myPrimitive(primitive), myTransform(Transform)
{

}
void GfxCmd_DrawDebugPrimitive::ExecuteAndDestroy()
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

GfxCmd_RenderSkeletalMeshShadow::GfxCmd_RenderSkeletalMeshShadow(
	const RenderData& aMesh,
	const Matrix& aTransform,
	const Matrix* aBoneTransformList,
	unsigned int aNumBones) :
	GfxCmd_RenderSkeletalMesh(aMesh, aTransform, aBoneTransformList, aNumBones)
{
}

void GfxCmd_RenderSkeletalMeshShadow::ExecuteAndDestroy()
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
	RHI::Context->PSSetShader(nullptr, nullptr, 0);

	for (auto& aElement : myElementsData)
	{
		RHI::ConfigureInputAssembler(
			aElement.PrimitiveTopology,
			aElement.VertexBuffer,
			aElement.IndexBuffer,
			aElement.Stride,
			Vertex::InputLayout);
		RHI::DrawIndexed(aElement.NumIndices);
	}
}

GraphicsCommandList::GraphicsCommandList() = default;

GraphicsCommandList::~GraphicsCommandList()
{
	GraphicsCommandListIterator it(*this);
	while (it.HasCommand())
	{
		GraphicCommandBase* cmd = it.Next();
 		cmd->Destroy();
	}
	delete[] myData;
	myData = nullptr;
	myRoot = nullptr;
	myLink = nullptr;
}

void GraphicsCommandList::Initialize(size_t aSize)
{
	mySize = aSize;
	myData = new uint8_t[mySize];
	memset(myData, 0, mySize);

	myRoot = reinterpret_cast<GraphicCommandBase*>(myData);
	myLink = &myRoot;
}

void GraphicsCommandList::Execute()
{
	if (!isExecuting && !isFinished)
	{
		isExecuting = true;
		GraphicsCommandListIterator it(*this);
		while (it.HasCommand())
		{
			GraphicCommandBase* cmd = it.Next();
			cmd->ExecuteAndDestroy();
		}
		isFinished = true;
		isExecuting = false;
	}
}

void GraphicsCommandList::Reset()
{
	if (isFinished)
	{
		memset(myData, 0, mySize);
		myRoot = reinterpret_cast<GraphicCommandBase*>(myData);
		myLink = &myRoot;
		myCursor = 0;
	}
}

GraphicsCommandListIterator::GraphicsCommandListIterator(const GraphicsCommandList& lst)
{
	myPtr = lst.myRoot;
}

GraphicCommandBase* GraphicsCommandListIterator::Next()
{
	GraphicCommandBase* cmd = myPtr;
	myPtr = cmd->Next;
	return cmd;
}
