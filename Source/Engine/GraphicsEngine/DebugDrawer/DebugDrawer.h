#pragma once
#include <Engine/GraphicsEngine/Objects/Shader.h>
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.hpp>
#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>

//Credited for work: Daniel Borghammar & Simon Nilsson 
struct DebugVertex
{
	DebugVertex(Vector3f aPosition,Vector3f aColor = {1.f,0.f,0.f}) : Position(aPosition.x,aPosition.y,aPosition.z,1),Color(aColor.x,aColor.y,aColor.z,1) {};
	Vector4f Position = {0,0,0,0};
	Vector4f Color = {0,0,0,0};

	static const std::vector<D3D11_INPUT_ELEMENT_DESC> InputLayoutDescription;
	static ComPtr<ID3D11InputLayout> InputLayout;
};

class DebugDrawer
{
	struct Primitive
	{
		Matrix Transform;
		std::vector<DebugVertex> Vertices{};
		std::vector<unsigned int> Indices{};
	};

public:
	struct PrimitiveHandle
	{
	public:
		friend class DebugDrawer;
		//const bool IsValid() const;
	private:
		size_t myValue = 0;
	};

	static DebugDrawer& Get();

	bool Initialize();
	void SetDebugPrimitiveTransform(const PrimitiveHandle& aHandle,const Matrix& aTransform); 
	void RemoveDebugPrimitive(PrimitiveHandle& aHandle);

	void Update(float aDeltaTime);
	void Render();

	PrimitiveHandle AddDebugLine(const Vector3f& aStart,const Vector3f& aFinish,const Vector3f& aColor = {1.f,0.f,0.f},const float lifetime = 0.0f);
	PrimitiveHandle AddDebugGizmo(const Vector3f& aCenter,const float aLength,const float lifetime = 0.0f);
	//PrimitiveHandle AddDebugArrow(const Vector3f& aStart, const CU::Vector3f& aFinish, const float aHeadSize, const CU::Vector3f& aColor);
	PrimitiveHandle AddDebugBox(const Vector3f& aMin,const Vector3f& aMax/*, Rotator*/,const Vector3f& aColor = {1.f,0.f,0.f},const float lifetime = 0.0f);
	//PrimitiveHandle AddDebugCircle(const CU::Vector3f& aCenter, const float aRadius/*, Rotator*/, const CU::Vector3f& aColor);
	//PrimitiveHandle AddDebugSphere(const CU::Vector3f& aCenter, const float aRadius/*, Rotator*/, const CU::Vector3f& aColor);
	//PrimitiveHandle AddDebugCylinder(const CU::Vector3f& aCenter, const float aHalfHeight, const float aRadius/*, Rotator*/, const CU::Vector3f& aColor);
	//PrimitiveHandle AddDebugCapsule(const CU::Vector3f& aCenter, const float aHalfHeight, const float aRadius/*, Rotator*/, const CU::Vector3f& aColor);
	//PrimitiveHandle AddDebugCone(const CU::Vector3f& aCenter, const float aHalfHeight, const float aStartRadius, float anEndRadius/*, Rotator*/, const CU::Vector3f& aColor);
	PrimitiveHandle AddDebugGrid(const Vector3f& aCenter,const float anExtent,const unsigned int someNumCells/*, Rotator*/,const  Vector3f& aColor = {1.f,0.f,0.f},const float lifetime = 0.0f);
	//PrimitiveHandle AddDebugGridEx(const CU::Vector3f& aCenter, const float anExtent, const unsigned int someNumGridLines/*, Rotator*/, const CU::Vector3f& aColor);

private:
	PrimitiveHandle CreatePrimitiveHandle(const Primitive& aPrimitive,float lifetime = 0.0f);

	std::unordered_map<size_t,Primitive> myDebugPrimitives;
	std::unordered_map<size_t,float> myDebugLifetime;
	size_t myNextIndex = 0;

	ComPtr<ID3D11Buffer> myLineVertexBuffer;
	ComPtr<ID3D11Buffer> myLineIndexBuffer;
	size_t myNumLineIndices = 0;

	std::shared_ptr<Shader> myLineVS;
	std::shared_ptr<Shader> myLinePS;
	bool myPrimitiveListDirty = false;
};
