#include "GraphicsEngine.pch.h"
#include "DebugDrawer.h"

#include "../Shaders/Include/LineDrawer_PS.h"
#include "../Shaders/Include/LineDrawer_VS.h"  
const std::vector<D3D12_INPUT_ELEMENT_DESC> DebugVertex::InputLayoutDescription =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
};
//ComPtr<ID3D11InputLayout> DebugVertex::InputLayout;



DebugDrawer& DebugDrawer::Get()
{
	static DebugDrawer singleton;
	return singleton;
}

bool DebugDrawer::Initialize()
{
	// Load shaders from memory
	//myLineVS = std::make_shared<Shader>();
	/*RHI::LoadShaderFromMemory(
		myLineVS.get(),
		L"LineVertexShader",
		BuiltIn_LineDrawer_VS_ByteCode,
		sizeof(BuiltIn_LineDrawer_VS_ByteCode)
	);*/

	//myLinePS = std::make_shared<Shader>();
	/*RHI::LoadShaderFromMemory(
		myLinePS.get(),
		L"LinePixelShader",
		BuiltIn_LineDrawer_PS_ByteCode,
		sizeof(BuiltIn_LineDrawer_PS_ByteCode)
	);

	//Create DebugVertex input layout
	/*RHI::CreateInputLayout(
		DebugVertex::InputLayout,
		DebugVertex::InputLayoutDescription,
		BuiltIn_LineDrawer_VS_ByteCode,
		sizeof(BuiltIn_LineDrawer_VS_ByteCode)
	);*/

	//// Create dynamic vertex and index buffer
	//if (!(
	//	RHI::CreateDynamicVertexBuffer(myLineVertexBuffer,65536,sizeof(DebugVertex)) &&
	//	RHI::CreateDynamicIndexBuffer(myLineIndexBuffer,65536)
	//	))
	//{
	//	Logger::Err("Failed to initialize the myLineVertexBuffer!");
	//	return false;
	//}
	return true;
}

void DebugDrawer::SetDebugPrimitiveTransform(const PrimitiveHandle& aHandle,const Matrix& aTransform)
{
	if (myDebugPrimitives.contains(aHandle.myValue))
	{
		myDebugPrimitives[aHandle.myValue].Transform = aTransform;
		myPrimitiveListDirty = true;
	}
}

void DebugDrawer::RemoveDebugPrimitive(PrimitiveHandle& aHandle)
{
	if (myDebugPrimitives.find(aHandle.myValue) != myDebugPrimitives.end())
	{
		myDebugPrimitives.erase(aHandle.myValue);
		myPrimitiveListDirty = true;
	}
}

void DebugDrawer::Update(float aDeltaTime)
{
	for (auto it = myDebugLifetime.begin(); it != myDebugLifetime.end(); ++it)
	{
		it->second -= aDeltaTime;
		if (it->second < 0.f)
		{
			myDebugPrimitives.erase(it->first);
			myPrimitiveListDirty = true;
		}
	}
}

void DebugDrawer::Render()
{
	//if (myPrimitiveListDirty)
	//{
	//	myNumLineIndices = 0;

	//	D3D11_MAPPED_SUBRESOURCE vxResource{};
	//	D3D11_MAPPED_SUBRESOURCE ixResource{};

	//	/*RHI::Context->Map(myLineVertexBuffer.Get(),0,D3D11_MAP_WRITE_DISCARD,0,&vxResource);
	//	RHI::Context->Map(myLineIndexBuffer.Get(),0,D3D11_MAP_WRITE_DISCARD,0,&ixResource);*/

	//	size_t currentVxOffset = 0;
	//	size_t currentIxOffset = 0;

	//	for (auto it = myDebugPrimitives.begin(); it != myDebugPrimitives.end(); ++it)
	//	{
	//		Primitive& currentPrimitive = it->second;

	//		DebugVertex* vxPtr = static_cast<DebugVertex*>(vxResource.pData) + currentVxOffset;

	//		for (size_t v = 0; v < currentPrimitive.Vertices.size(); ++v)
	//		{
	//			vxPtr[v] = currentPrimitive.Vertices[v];
	//			//Vector3f position = Matrix::ReadPosition(currentPrimitive.Transform);
	//			//vxPtr[v].Position = Vector4f(currentPrimitive.Vertices[v].Position + Vector4f(position.x,position.y,position.z,1.0f));
	//			vxPtr[v].Position = Vector4f(currentPrimitive.Vertices[v].Position * currentPrimitive.Transform);
	//			vxPtr[v].Position.w = 1.0f;
	//		}

	//		unsigned int* ixPtr = static_cast<unsigned int*>(ixResource.pData) + currentIxOffset;
	//		const unsigned int vxOffset = static_cast<unsigned int>(currentVxOffset);

	//		for (size_t i = 0; i < currentPrimitive.Indices.size(); ++i)
	//		{
	//			ixPtr[i] = currentPrimitive.Indices[i] + vxOffset;
	//		}

	//		currentVxOffset += currentPrimitive.Vertices.size();
	//		currentIxOffset += currentPrimitive.Indices.size();
	//	}

	//	myNumLineIndices = currentIxOffset;

	//	//RHI::Context->Unmap(myLineVertexBuffer.Get(),0);
	//	//RHI::Context->Unmap(myLineIndexBuffer.Get(),0);

	//	myPrimitiveListDirty = false;
	//}

	//if (myNumLineIndices > 0)
	//{/*
	//	RHI::ConfigureInputAssembler(
	//		D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
	//		myLineVertexBuffer,
	//		myLineIndexBuffer,
	//		sizeof(DebugVertex),
	//		DebugVertex::InputLayout
	//	);*/

	//	RHI::SetVertexShader(myLineVS.get());
	//	RHI::SetPixelShader(myLinePS.get());
	//	RHI::DrawIndexed(static_cast<UINT>(myNumLineIndices));
	//}
}

DebugDrawer::PrimitiveHandle DebugDrawer::AddDebugLine(const Vector3f& aStart,const Vector3f& aFinish,const Vector3f& aColor,const float lifetime)
{
	Primitive primitive{};
	primitive.Vertices.push_back(DebugVertex(aStart,aColor));
	primitive.Vertices.push_back(DebugVertex(aFinish,aColor));
	primitive.Indices.push_back(0);
	primitive.Indices.push_back(1);

	return CreatePrimitiveHandle(primitive,lifetime);
}

DebugDrawer::PrimitiveHandle DebugDrawer::AddDebugGizmo(const Vector3f& aCenter,const float aLength,const float lifetime)
{
	Primitive primitive{};
	//X 
	primitive.Vertices.push_back(DebugVertex(aCenter,{ 1,0,0 }));
	primitive.Vertices.push_back(DebugVertex({ aLength,0,0 },{ 1,0,0 }));
	primitive.Indices.push_back(0);
	primitive.Indices.push_back(1);

	//Y
	primitive.Vertices.push_back(DebugVertex(aCenter,{ 0,1,0 }));
	primitive.Vertices.push_back(DebugVertex({ 0,aLength,0 },{ 0,1,0 }));
	primitive.Indices.push_back(2);
	primitive.Indices.push_back(3);

	//Z
	primitive.Vertices.push_back(DebugVertex(aCenter,{ 0,0,1 }));
	primitive.Vertices.push_back(DebugVertex({ 0,0,aLength },{ 0,0,1 }));
	primitive.Indices.push_back(4);
	primitive.Indices.push_back(5);

	return CreatePrimitiveHandle(primitive,lifetime);
}

DebugDrawer::PrimitiveHandle DebugDrawer::AddDebugBox(const Vector3f& aMin,const Vector3f& aMax,const Vector3f& aColor,const float lifetime)
{
	Primitive primitive{};

	Vector3f min = aMin;
	Vector3f max = aMax;
	Vector3f vertex = min;

	primitive.Vertices.reserve(8);
	primitive.Vertices.push_back(DebugVertex(vertex,aColor));
	vertex.x = max.x;
	primitive.Vertices.push_back(DebugVertex(vertex,aColor));
	vertex.y = max.y;
	primitive.Vertices.push_back(DebugVertex(vertex,aColor));
	vertex.z = max.z;
	primitive.Vertices.push_back(DebugVertex(vertex,aColor));
	vertex.y = min.y;
	primitive.Vertices.push_back(DebugVertex(vertex,aColor));
	vertex.x = min.x;
	primitive.Vertices.push_back(DebugVertex(vertex,aColor));
	vertex.y = max.y;
	primitive.Vertices.push_back(DebugVertex(vertex,aColor));
	vertex.z = min.z;
	primitive.Vertices.push_back(DebugVertex(vertex,aColor));


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

DebugDrawer::PrimitiveHandle DebugDrawer::AddDebugGrid(const Vector3f& aCenter,const float anExtent,const unsigned int someNumCells,const Vector3f& aColor,const float lifetime)
{
	Primitive primitive{};

	primitive.Vertices.reserve(static_cast<size_t>(someNumCells) * 4 + 4);
	primitive.Indices.reserve(static_cast<size_t>(someNumCells) * 4 + 4);

	DebugVertex vertex(Vector3f(),aColor);

	float offset = (anExtent * 2.f) / someNumCells;

	Vector4f aStartPos = Vector4f(aCenter.x,aCenter.y,aCenter.z,1.0f);
	aStartPos.x -= anExtent;
	aStartPos.z -= anExtent;
	Vector4f anEndPos = aStartPos;
	anEndPos.x += anExtent * 2;

	for (unsigned int i = 0; i <= someNumCells * 2; i += 2)
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

	for (unsigned int i = 0; i <= someNumCells * 2; i += 2)
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


	if (lifetime > 0.0f)
	{
		myDebugLifetime[handle.myValue] = lifetime;
	}

	myDebugPrimitives[handle.myValue] = aPrimitive;
	myPrimitiveListDirty = true;
	return handle;
}