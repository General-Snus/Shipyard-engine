#include "GraphicsEngine.pch.h"
//#include <Engine/GraphicsEngine/InterOp/RHI.h>
//#include "InstanceRenderer.h"
//
//#include <Engine\AssetManager\Objects\BaseAssets\MaterialAsset.h>
//#include <Tools\Optick\include\optick.h>
//
//void InstanceRenderer::Init()
//{
//}
//
//void InstanceRenderer::Execute(bool isShadowPass)
//{
//	OPTICK_EVENT();
//	if (isShadowPass)
//	{
//		//RHI::Context->PSSetShader(nullptr,nullptr,0);
//	}
//	else
//	{
//		G_Buffer& gBuffer = RENDERER.myG_Buffer;
//		gBuffer.UseGBufferShader();
//	}
//
//	for (const auto& [key,i] : instanceRenderData)
//	{
//		if (i->myMesh->isLoadedComplete)
//		{
//			OPTICK_EVENT("Mesh");
//			ObjectBuffer& objectBuffer = RENDERER.myObjectBuffer;
//			objectBuffer.myTransform = Matrix();
//			objectBuffer.MaxExtents = i->myMesh->MaxBox;
//			objectBuffer.MinExtents = i->myMesh->MinBox;
//			objectBuffer.hasBone = false;
//			objectBuffer.isInstanced = true;
//			//RHI::UpdateConstantBufferData(RENDERER.myObjectBuffer);
//			if (!isShadowPass)
//			{
//				i->myMesh->UpdateInstanceBuffer();
//			}
//			for (const auto& aElement : i->myMesh->Elements)
//			{
//				if (!isShadowPass)
//				{
//					if (!i->overrideMaterial.empty())
//					{
//						for (const auto& mat : i->overrideMaterial)
//						{
//							mat->Update();
//						}
//					}
//					else
//					{
//						if (auto materialPtr = i->myMesh->GetMaterialList().at(aElement.MaterialIndex))
//						{
//							i->myMesh->GetMaterialList().at(aElement.MaterialIndex)->Update();
//						}
//						else
//						{
//							RENDERER.defaultMaterial->Update();
//						}
//					}
//				}
//
//				/*const std::vector<ComPtr<ID3D12Resource>> vxBuffers
//				{
//					aElement.VertexBuffer,
//					i->myMesh->myInstanceBuffer
//				};
//
//				const std::vector<unsigned> vfBufferStrides
//				{
//					aElement.Stride,
//					sizeof(Matrix)
//				};*/
//
//				/*RHI::ConfigureInputAssembler(
//					aElement.PrimitiveTopology,
//					vxBuffers,
//					aElement.IndexBuffer,
//					vfBufferStrides,
//					Vertex::InputLayout);
//				OPTICK_EVENT("Element");
//				RHI::DrawIndexedInstanced(aElement.NumIndices,static_cast<unsigned>(i->myMesh->myInstances.size()));*/
//			}
//		}
//	}
//}
//
//void InstanceRenderer::AddInstance(const std::shared_ptr<Mesh>& aRenderData)
//{
//	instanceRenderData[aRenderData] = aRenderData;
//}
//
//void InstanceRenderer::Clear()
//{
//	for (const auto& [key,value] : instanceRenderData)
//	{
//		value->myMesh->myInstances.clear();
//	}
//	instanceRenderData.clear();
//}
