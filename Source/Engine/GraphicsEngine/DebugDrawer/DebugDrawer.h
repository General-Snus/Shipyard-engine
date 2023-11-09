#pragma once
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.hpp>
#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>


namespace Debug
{
	struct DebugVertex
	{
		DebugVertex( Vector3f aPosition, Vector3f aColor,float aThicc) : Position(aPosition.x,aPosition.y,aPosition.z,1),Color(aColor.x,aColor.y,aColor.z,1),thickness(aThicc) {  };
		Vector4f Position = {0,0,0,0};
		Vector4f Color = {0,0,0,0};
		float thickness = 0;

		static const std::vector<D3D11_INPUT_ELEMENT_DESC> InputLayoutDescription;
		static ComPtr<ID3D11InputLayout> InputLayout;
	};

	struct DebugPrimitive
	{
		DebugPrimitive() = default;
		DebugPrimitive(const std::vector<DebugVertex>& myVertexs,const std::vector<unsigned int>& myIndices);

		ComPtr<ID3D11Buffer> VertexBuffer;
		ComPtr<ID3D11Buffer> IndexBuffer; 
		UINT NumVertices = 0;
		UINT NumIndices = 0; 
		float thickness;
	};


	class DebugDrawer
	{
	public:
		static DebugDrawer& Get(); 
	};
}

