#include "GraphicsEngine.pch.h"

#include "DebugDrawer.h"
#include "Renderer.h"
#include <DirectX/DX12/Graphics/GPU.h>
#include <DirectX/DX12/Graphics/PSO.h>
#include <DirectX/DX12/Graphics/CommandQueue.h>
#include <DirectX/DX12/Graphics/CommandList.h>

#include <Engine/AssetManager/Objects/BaseAssets/ShipyardShader.h>
#include <Engine/AssetManager/Objects/BaseAssets/MaterialAsset.h>

#include "DirectX/DX12/Graphics/Resources/IndexBuffer.h"
#include "DirectX/DX12/Graphics/Resources/VertexBuffer.h"



bool DebugDrawer::Initialize()
{
	vertexBuffer = std::make_shared<VertexResource>("DebugDrawerVertexBuffer");
	indexBuffer = std::make_shared<IndexResource>("DebugDrawerIndexBuffer");


	const auto commandQueue = GPUInstance.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
	const auto commandList = commandQueue->GetCommandList();

	std::vector<DebugVertex> vertices;
	vertices.resize(65536);

	std::vector<uint32_t> indicies;
	indicies.resize(65536);

	commandList->CreateVertexBuffer<DebugVertex>(*vertexBuffer,vertices,CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,
																								D3D12_MEMORY_POOL_L0));
	commandList->CreateIndexBuffer(*indexBuffer,indicies,
								   CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK,
														   D3D12_MEMORY_POOL_L0));
	commandQueue->ExecuteCommandList(commandList);
	return true;
}

void DebugDrawer::SetDebugPrimitiveTransform(const PrimitiveHandle& aHandle,const Matrix& aTransform)
{
	if(myDebugPrimitives.contains(aHandle.myValue))
	{
		myDebugPrimitives[aHandle.myValue].Transform = aTransform;
		myPrimitiveListDirty = true;
	}
}

void DebugDrawer::RemoveDebugPrimitive(PrimitiveHandle& aHandle)
{
	if(myDebugPrimitives.contains(aHandle.myValue))
	{
		myDebugPrimitives.erase(aHandle.myValue);
		myPrimitiveListDirty = true;
	}
}

void DebugDrawer::Update(float aDeltaTime)
{
	OPTICK_EVENT();
	for(auto it = myDebugLifetime.begin(); it != myDebugLifetime.end(); ++it)
	{
		it->second -= aDeltaTime;
		if(it->second < 0.f)
		{
			myDebugPrimitives.erase(it->first);
			myPrimitiveListDirty = true;
		}
	}
}

void DebugDrawer::Render(std::shared_ptr<CommandList> commandList)
{
	if(myPrimitiveListDirty)
	{
		myNumLineIndices = 0;
		CD3DX12_RANGE readRange(0,0);

		auto vertBuffer = vertexBuffer->Resource();
		auto indicesBuffer = indexBuffer->Resource();

		DebugVertex* vxMappedData = nullptr;
		HRESULT      hr = vertBuffer->Map(0,&readRange,reinterpret_cast<void**>(&vxMappedData));
		if(FAILED(hr))
		{
			return;
		}
		unsigned int* ixMappedData = nullptr;
		hr = indicesBuffer->Map(0,&readRange,reinterpret_cast<void**>(&ixMappedData));
		if(FAILED(hr))
		{
			return;
		}

		size_t currentVxOffset = 0;
		size_t currentIxOffset = 0;

		for(auto it = myDebugPrimitives.begin(); it != myDebugPrimitives.end(); ++it)
		{
			Primitive& currentPrimitive = it->second;
			DebugVertex* vxPtr = vxMappedData + currentVxOffset;
			unsigned int* ixPtr = ixMappedData + currentIxOffset;
			const unsigned int vxOffset = static_cast<unsigned int>(currentVxOffset);

			for(size_t v = 0; v < currentPrimitive.Vertices.size(); ++v)
			{
				vxPtr[v] = currentPrimitive.Vertices[v];
				vxPtr[v].Position = Vector4f(currentPrimitive.Vertices[v].Position * currentPrimitive.Transform);
				vxPtr[v].Position.w = 1.0f;
			}

			for(size_t i = 0; i < currentPrimitive.Indices.size(); ++i)
			{
				ixPtr[i] = currentPrimitive.Indices[i] + vxOffset;
			}

			currentVxOffset += currentPrimitive.Vertices.size();
			currentIxOffset += currentPrimitive.Indices.size();
		}

		myNumLineIndices = currentIxOffset;

		vertBuffer->Unmap(0,&readRange);
		indicesBuffer->Unmap(0,&readRange);
		myPrimitiveListDirty = false;
	}

	if(myNumLineIndices > 0)
	{
		MaterialBuffer materialBuffer;

		materialBuffer.vertexBufferIndex = vertexBuffer->GetHandle(ViewType::SRV).heapOffset;
		materialBuffer.vertexOffset = 0;

		assert(materialBuffer.vertexBufferIndex != -1 && "HEAP INDEX OUT OF BOUND");
		assert(materialBuffer.vertexOffset != -1 && "HEAP INDEX OUT OF BOUND");

		const auto& alloc = GPUInstance.m_GraphicsMemory->AllocateConstant(materialBuffer);
		commandList->GetGraphicsCommandList()->SetGraphicsRootConstantBufferView(
			REG_DefaultMaterialBuffer,alloc.GpuAddress());

		commandList->ConfigureInputAssembler(D3D_PRIMITIVE_TOPOLOGY_LINELIST,*indexBuffer);
		const auto& pso = RENDERER.GetPSOCache().GetState(PSOCache::ePipelineStateID::DebugDraw);
		commandList->SetPipelineState(*pso);
		commandList->GetGraphicsCommandList()->DrawIndexedInstanced(static_cast<UINT>(myNumLineIndices),1,0,0,0);
	}
}

DebugDrawer::PrimitiveHandle DebugDrawer::AddDebugLine(const Vector3f& aStart,const Vector3f& aFinish,
													   const Vector3f& aColor,const float     lifetime)
{
	Primitive primitive{};
	primitive.Vertices.push_back(DebugVertex(aStart,Vector4f(aColor,1.0f)));
	primitive.Vertices.push_back(DebugVertex(aFinish,Vector4f(aColor,1.0f)));
	primitive.Indices.push_back(0);
	primitive.Indices.push_back(1);

	return CreatePrimitiveHandle(primitive,lifetime);
}

DebugDrawer::PrimitiveHandle DebugDrawer::DebugRay(const Vector3f& aStart,const Vector3f& aDirection,float length,
												   const Vector3f& aColor,const float     lifetime)
{
	assert(length > 0.0f && "Length must be greater than 0");

	Primitive primitive{};
	primitive.Vertices.emplace_back(aStart,Vector4f(aColor,1.0f));
	primitive.Vertices.emplace_back(aDirection.GetNormalized() * length,Vector4f(aColor,1.0f));
	primitive.Indices.emplace_back(0);
	primitive.Indices.emplace_back(1);

	return CreatePrimitiveHandle(primitive,lifetime);
}
DebugDrawer::PrimitiveHandle DebugDrawer::AddDebugGizmo(const Vector3f& aCenter,const float aLength,
														const float     lifetime)
{
	Primitive primitive{};
	//X 
	primitive.Vertices.push_back(DebugVertex(aCenter,{1,0,0,1}));
	primitive.Vertices.push_back(DebugVertex({aLength,0,0},{1,0,0,1}));
	primitive.Indices.push_back(0);
	primitive.Indices.push_back(1);

	//Y
	primitive.Vertices.push_back(DebugVertex(aCenter,{0,1,0,1}));
	primitive.Vertices.push_back(DebugVertex({0,aLength,0},{0,1,0,1}));
	primitive.Indices.push_back(2);
	primitive.Indices.push_back(3);

	//Z
	primitive.Vertices.push_back(DebugVertex(aCenter,{0,0,1,1}));
	primitive.Vertices.push_back(DebugVertex({0,0,aLength},{0,0,1,1}));
	primitive.Indices.push_back(4);
	primitive.Indices.push_back(5);

	return CreatePrimitiveHandle(primitive,lifetime);
}

DebugDrawer::PrimitiveHandle DebugDrawer::AddDebugBox(const Vector3f& aMin,const Vector3f& aMax,
													  const Vector3f& aColor,const float   lifetime)
{

	Primitive primitive{};
	const Vector3f min = aMin;
	const Vector3f max = aMax;
	Vector3f vertex = min;

	primitive.Vertices.reserve(8);
	primitive.Vertices.emplace_back(vertex,Vector4f(aColor,1.0f));
	vertex.x = max.x;
	primitive.Vertices.emplace_back(vertex,Vector4f(aColor,1.0f));
	vertex.y = max.y;
	primitive.Vertices.emplace_back(vertex,Vector4f(aColor,1.0f));
	vertex.z = max.z;
	primitive.Vertices.emplace_back(vertex,Vector4f(aColor,1.0f));
	vertex.y = min.y;
	primitive.Vertices.emplace_back(vertex,Vector4f(aColor,1.0f));
	vertex.x = min.x;
	primitive.Vertices.emplace_back(vertex,Vector4f(aColor,1.0f));
	vertex.y = max.y;
	primitive.Vertices.emplace_back(vertex,Vector4f(aColor,1.0f));
	vertex.z = min.z;
	primitive.Vertices.emplace_back(vertex,Vector4f(aColor,1.0f));


	primitive.Indices.reserve(24);
	primitive.Indices.push_back(0);
	primitive.Indices.push_back(1);
	primitive.Indices.push_back(1);
	primitive.Indices.push_back(2);
	primitive.Indices.push_back(2);
	primitive.Indices.push_back(7);
	primitive.Indices.push_back(7);
	primitive.Indices.push_back(0);

	primitive.Indices.push_back(3);
	primitive.Indices.push_back(4);
	primitive.Indices.push_back(4);
	primitive.Indices.push_back(5);
	primitive.Indices.push_back(5);
	primitive.Indices.push_back(6);
	primitive.Indices.push_back(6);
	primitive.Indices.push_back(3);

	primitive.Indices.push_back(0);
	primitive.Indices.push_back(5);
	primitive.Indices.push_back(1);
	primitive.Indices.push_back(4);
	primitive.Indices.push_back(2);
	primitive.Indices.push_back(3);
	primitive.Indices.push_back(6);
	primitive.Indices.push_back(7);

	return CreatePrimitiveHandle(primitive,lifetime);
}

//DebugDrawer::PrimitiveHandle DebugDrawer::AddDebugCircle(const Vector3f & aCenter, const float aRadius, const Vector3f & aColor)
//{
//	Primitive primitive{};
//
//	const int numberOfPoints = 32;
//	primitive.Vertices.reserve(numberOfPoints);
//	primitive.Indices.reserve(static_cast<size_t>(numberOfPoints * 2));
//
//	Vector4f offset(aRadius, 0, 0, 0);
//	Matrix4x4<float> rotator = Matrix4x4<float>::CreateRotationAroundY(6.283f / numberOfPoints);
//
//	DebugVertex vertex;
//	vertex.Color = aColor;
//	for (size_t i = 0; i < numberOfPoints; i++)
//	{
//		Vector3f position = aCenter + Vector3f(offset.x, offset.y, offset.z);
//		vertex.Position = Vector4f(position.x, position.y, position.z, 1.0f);
//		primitive.Vertices.push_back(vertex);
//		offset = offset * rotator;
//		primitive.Indices.push_back(i);
//		primitive.Indices.push_back((i + 1) % numberOfPoints);
//	}
//
//	return CreatePrimitiveHandle(primitive);
//}
//
//DebugDrawer::PrimitiveHandle DebugDrawer::AddDebugSphere(const Vector3f & aCenter, const float aRadius, const Vector3f & aColor)
//{
//	Primitive primitive{};
//
//	const int numberOfPoints = 32;
//	primitive.Vertices.reserve(static_cast<size_t>(numberOfPoints * 3));
//	primitive.Indices.reserve(static_cast<size_t>(numberOfPoints * 6));
//
//	DebugVertex vertex;
//	vertex.Color = aColor;
//	// Around Y
//	Vector4f offset(aRadius, 0, 0, 0);
//	Matrix4x4<float> rotator = Matrix4x4<float>::CreateRotationAroundY(6.283f / numberOfPoints);
//	for (size_t i = 0; i < numberOfPoints; i++)
//	{
//		Vector3f position = aCenter + Vector3f(offset.x, offset.y, offset.z);
//		vertex.Position = Vector4f(position.x, position.y, position.z, 1.0f);
//		primitive.Vertices.push_back(vertex);
//		offset = offset * rotator;
//		primitive.Indices.push_back(i);
//		primitive.Indices.push_back((i + 1) % numberOfPoints);
//	}
//
//	// Around Z
//	rotator = Matrix4x4<float>::CreateRotationAroundZ(6.283f / numberOfPoints);
//	for (size_t i = 0; i < numberOfPoints; i++)
//	{
//		Vector3f position = aCenter + Vector3f(offset.x, offset.y, offset.z);
//		vertex.Position = Vector4f(position.x, position.y, position.z, 1.0f);
//		primitive.Vertices.push_back(vertex);
//		offset = offset * rotator;
//		primitive.Indices.push_back(numberOfPoints + i);
//		primitive.Indices.push_back(numberOfPoints + ((i + 1) % numberOfPoints));
//	}
//
//
//	// Around X
//	offset = { 0, 0, aRadius, 0 };
//	rotator = Matrix4x4<float>::CreateRotationAroundX(6.283f / numberOfPoints);
//	for (size_t i = 0; i < numberOfPoints; i++)
//	{
//		Vector3f position = aCenter + Vector3f(offset.x, offset.y, offset.z);
//		vertex.Position = Vector4f(position.x, position.y, position.z, 1.0f);
//		primitive.Vertices.push_back(vertex);
//		offset = offset * rotator;
//		primitive.Indices.push_back((static_cast<size_t>(numberOfPoints) * 2) + i);
//		primitive.Indices.push_back((static_cast<size_t>(numberOfPoints) * 2) + ((i + 1) % numberOfPoints));
//	}
//
//	return CreatePrimitiveHandle(primitive);
//}
//
//DebugDrawer::PrimitiveHandle DebugDrawer::AddDebugCylinder(const Vector3f & aCenter, const float aHalfHeight, const float aRadius, const Vector3f & aColor)
//{
//	Primitive primitive{};
//
//	const int numberOfPoints = 32;
//	const int quarter = numberOfPoints / 2;
//	primitive.Vertices.reserve(static_cast<size_t>(numberOfPoints * 2));
//	primitive.Indices.reserve(static_cast<size_t>(numberOfPoints * 4 + 8));
//
//	Vector4f offset(aRadius, aHalfHeight, 0, 0);
//	Matrix4x4<float> rotator = Matrix4x4<float>::CreateRotationAroundY(6.283f / numberOfPoints);
//
//	DebugVertex vertex;
//	vertex.Color = aColor;
//	for (size_t i = 0; i < static_cast<size_t>(numberOfPoints) * 2; i += 2)
//	{
//		Vector3f position = aCenter + Vector3f(offset.x, offset.y, offset.z);
//		vertex.Position = Vector4f(position.x, position.y, position.z, 1.0f);
//		primitive.Vertices.push_back(vertex);
//		vertex.Position.y -= 2 * aHalfHeight;
//		primitive.Vertices.push_back(vertex);
//		offset = offset * rotator;
//		primitive.Indices.push_back(i);
//		primitive.Indices.push_back((i + 2) % (static_cast<size_t>(numberOfPoints * 2)));
//		primitive.Indices.push_back((i + 1));
//		primitive.Indices.push_back((i + 3) % (static_cast<size_t>(numberOfPoints * 2)));
//
//		if (i % quarter == 0)
//		{
//			primitive.Indices.push_back(i);
//			primitive.Indices.push_back((i + 1));
//		}
//	}
//
//	return CreatePrimitiveHandle(primitive);
//}

//DebugDrawer::PrimitiveHandle DebugDrawer::AddDebugCapsule(const Vector3f & aCenter, const float aHalfHeight, const float aRadius, const Vector3f & aColor)
//{
//	Primitive primitive{};
//
//	const int numberOfPoints = 32;
//	const int quarter = numberOfPoints / 2;
//	primitive.Vertices.reserve(static_cast<size_t>((numberOfPoints * 4) + 12)); // TODO: This might not be correct
//	primitive.Indices.reserve(static_cast<size_t>((numberOfPoints * 8) + 8));
//
//	Vector4f offset(aRadius, aHalfHeight, 0, 0);
//	Matrix4x4<float> rotator = Matrix4x4<float>::CreateRotationAroundY(6.283f / numberOfPoints);
//
//	DebugVertex vertex;
//	vertex.Color = aColor;
//	// Cylinder
//	for (size_t i = 0; i < static_cast<size_t>(numberOfPoints) * 2; i += 2)
//	{
//		Vector3f position = aCenter + Vector3f(offset.x, offset.y, offset.z);
//		vertex.Position = Vector4f(position.x, position.y, position.z, 1.0f);
//		primitive.Vertices.push_back(vertex);
//		vertex.Position.y -= 2 * aHalfHeight;
//		primitive.Vertices.push_back(vertex);
//		offset = offset * rotator;
//		primitive.Indices.push_back(i);
//		primitive.Indices.push_back((i + 2) % (static_cast<size_t>(numberOfPoints * 2)));
//		primitive.Indices.push_back((i + 1));
//		primitive.Indices.push_back((i + 3) % (static_cast<size_t>(numberOfPoints * 2)));
//
//		if (i % quarter == 0)
//		{
//			primitive.Indices.push_back(i);
//			primitive.Indices.push_back((i + 1));
//		}
//	}
//
//	unsigned int lastIndex = numberOfPoints * 2;
//
//	// Caps
//	offset = { aRadius, 0, 0, 0 };
//	Vector3f halfHeightVec = { 0, aHalfHeight, 0 };
//	rotator = Matrix4x4<float>::CreateRotationAroundZ(6.283f / numberOfPoints);
//	for (size_t i = 0; i < static_cast<size_t>(numberOfPoints); i += 2)
//	{
//		Vector3f position = aCenter + Vector3f(offset.x, offset.y, offset.z) + halfHeightVec;
//		vertex.Position = Vector4f(position.x, position.y, position.z, 1.0f);
//		primitive.Vertices.push_back(vertex);
//		position = aCenter - Vector3f(offset.x, offset.y, offset.z) + halfHeightVec;
//		vertex.Position = aCenter - (Vector3f(offset) + halfHeightVec);
//		primitive.Vertices.push_back(vertex);
//		offset = offset * rotator;
//		primitive.Indices.push_back(lastIndex + i);
//		primitive.Indices.push_back(lastIndex + i + 2);
//		primitive.Indices.push_back(lastIndex + i + 1);
//		primitive.Indices.push_back(lastIndex + i + 3);
//	}
//	Vector3f position = aCenter + Vector3f(offset.x, offset.y, offset.z) + halfHeightVec;
//	vertex.Position = Vector4f(position.x, position.y, position.z, 1.0f);
//	primitive.Vertices.push_back(vertex);
//	position = aCenter - Vector3f(offset.x, offset.y, offset.z) + halfHeightVec;
//	vertex.Position = aCenter - (Vector3f(offset) + halfHeightVec);
//	primitive.Vertices.push_back(vertex);
//
//	lastIndex += numberOfPoints + 2;
//
//	offset = { 0, 0, -aRadius, 0 };
//	rotator = Matrix4x4<float>::CreateRotationAroundX(6.283f / numberOfPoints);
//	for (size_t i = 0; i < static_cast<size_t>(numberOfPoints); i += 2)
//	{
//		Vector3f position = aCenter + Vector3f(offset.x, offset.y, offset.z) + halfHeightVec;
//		vertex.Position = Vector4f(position.x, position.y, position.z, 1.0f);
//		primitive.Vertices.push_back(vertex);
//		position = aCenter - Vector3f(offset.x, offset.y, offset.z) + halfHeightVec;
//		vertex.Position = aCenter - (Vector3f(offset) + halfHeightVec);
//		primitive.Vertices.push_back(vertex);
//		offset = offset * rotator;
//		primitive.Indices.push_back(lastIndex + i);
//		primitive.Indices.push_back(lastIndex + i + 2);
//		primitive.Indices.push_back(lastIndex + i + 1);
//		primitive.Indices.push_back(lastIndex + i + 3);
//	}
//	position = aCenter + Vector3f(offset.x, offset.y, offset.z) + halfHeightVec;
//	vertex.Position = Vector4f(position.x, position.y, position.z, 1.0f);
//	primitive.Vertices.push_back(vertex);
//	position = aCenter - Vector3f(offset.x, offset.y, offset.z) + halfHeightVec;
//	primitive.Vertices.push_back(vertex);
//
//	return CreatePrimitiveHandle(primitive);
//}
//
//DebugDrawer::PrimitiveHandle DebugDrawer::AddDebugCone(const Vector3f & aCenter, const float aHalfHeight, const float aStartRadius, float anEndRadius, const Vector3f & aColor)
//{
//	Primitive primitive{};
//
//	const int numberOfPoints = 32;
//	primitive.Vertices.reserve(numberOfPoints);
//	primitive.Indices.reserve(static_cast<size_t>(numberOfPoints * 2));
//
//	Matrix4x4<float> rotator = Matrix4x4<float>::CreateRotationAroundY(6.283f / numberOfPoints);
//
//	DebugVertex vertex;
//	vertex.Color = aColor;
//	if (aStartRadius == 0 || anEndRadius == 0)
//	{
//		const int quarter = numberOfPoints / 4;
//		Vector4f offset{ 0, 0, 0, 0 };
//		if (aStartRadius == 0)
//		{
//			offset.x = anEndRadius;
//			offset.y = aHalfHeight;
//		}
//		else
//		{
//			offset.x = aStartRadius;
//			offset.y = -aHalfHeight;
//		}
//
//		for (size_t i = 0; i < numberOfPoints; i++)
//		{
//			Vector3f position = aCenter + Vector3f(offset.x, offset.y, offset.z);
//			vertex.Position = Vector4f(position.x, position.y, position.z, 1.0f);
//			primitive.Vertices.push_back(vertex);
//			offset = offset * rotator;
//			primitive.Indices.push_back(i);
//			primitive.Indices.push_back((i + 1) % numberOfPoints);
//
//			if (i % quarter == 0)
//			{
//				primitive.Indices.push_back(i);
//				primitive.Indices.push_back(numberOfPoints);
//			}
//		}
//		Vector3f position = aCenter + Vector3f(0.f, -offset.y, 0.f);
//		vertex.Position = aCenter + Vector3f(0, -offset.y, 0);
//		primitive.Vertices.push_back(vertex);
//
//		return CreatePrimitiveHandle(primitive);
//	}
//
//	Vector4f topOffset(anEndRadius, aHalfHeight, 0, 0);
//	Vector4f botOffset(aStartRadius, -aHalfHeight, 0, 0);
//	const int quarter = numberOfPoints / 2;
//
//	for (size_t i = 0; i < static_cast<size_t>(numberOfPoints) * 2; i += 2)
//	{
//		vertex.Position = Vector4f(topOffset.x, topOffset.y, topOffset.z, 1.0f);
//		primitive.Vertices.push_back(vertex);
//		vertex.Position = Vector4f(botOffset.x, botOffset.y, botOffset.z, 1.0f);
//		primitive.Vertices.push_back(vertex);
//		topOffset = topOffset * rotator;
//		botOffset = botOffset * rotator;
//		primitive.Indices.push_back(i);
//		primitive.Indices.push_back((i + 2) % (static_cast<size_t>(numberOfPoints * 2)));
//		primitive.Indices.push_back((i + 1));
//		primitive.Indices.push_back((i + 3) % (static_cast<size_t>(numberOfPoints * 2)));
//
//		if (i % quarter == 0)
//		{
//			primitive.Indices.push_back(i);
//			primitive.Indices.push_back((i + 1));
//		}
//	}
//
//	return CreatePrimitiveHandle(primitive);
//}

DebugDrawer::PrimitiveHandle DebugDrawer::AddDebugGrid(const Vector3f& aCenter,const float          anExtent,
													   const unsigned int someNumCells,const Vector3f& aColor,
													   const float        lifetime)
{
	Primitive primitive{};

	primitive.Vertices.reserve(static_cast<size_t>(someNumCells) * 4 + 4);
	primitive.Indices.reserve(static_cast<size_t>(someNumCells) * 4 + 4);

	DebugVertex vertex(Vector3f(),Vector4f(aColor,1.0f));

	const float offset = (anExtent * 2.f) / static_cast<float>(someNumCells);

	auto aStartPos = Vector4f(aCenter.x,aCenter.y,aCenter.z,1.0f);
	aStartPos.x -= anExtent;
	aStartPos.z -= anExtent;
	Vector4f anEndPos = aStartPos;
	anEndPos.x += anExtent * 2;

	for(unsigned int i = 0; i <= someNumCells * 2; i += 2)
	{
		vertex.Position = aStartPos;
		primitive.Vertices.push_back(vertex);
		vertex.Position = anEndPos;
		primitive.Vertices.push_back(vertex);

		aStartPos.z += offset;
		anEndPos.z += offset;

		primitive.Indices.push_back(i);
		primitive.Indices.push_back(i + 1);
	}

	aStartPos = Vector4f(aCenter,1);
	aStartPos.x -= anExtent;
	aStartPos.z -= anExtent;
	anEndPos = aStartPos;
	anEndPos.z += anExtent * 2;

	for(unsigned int i = 0; i <= someNumCells * 2; i += 2)
	{
		vertex.Position = aStartPos;
		primitive.Vertices.push_back(vertex);
		vertex.Position = anEndPos;
		primitive.Vertices.push_back(vertex);

		aStartPos.x += offset;
		anEndPos.x += offset;

		primitive.Indices.push_back((someNumCells * 2 + 2) + i);
		primitive.Indices.push_back((someNumCells * 2 + 2) + i + 1);
	}

	return CreatePrimitiveHandle(primitive,lifetime);
}

DebugDrawer::PrimitiveHandle DebugDrawer::CreatePrimitiveHandle(const Primitive& aPrimitive,float lifetime)
{
	PrimitiveHandle handle;
	handle.myValue = myNextIndex++;


	if(lifetime > 0.0f)
	{
		myDebugLifetime[handle.myValue] = lifetime;
	}

	myDebugPrimitives[handle.myValue] = aPrimitive;
	myPrimitiveListDirty = true;
	return handle;
}

DebugVertex::DebugVertex() = default;
