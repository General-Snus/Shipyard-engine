#include <DirectXMath.h>
#include "Matrix4x4.h" 
#include "Tools/Assimp/include/assimp/matrix4x4.h" 

template<>
Matrix4x4<float>::Matrix4x4(const DirectX::XMMATRIX* aMatrix)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			arr[i][j] = aMatrix->r[i].m128_f32[j];
		}
	}
}

template<>
Matrix4x4<float>::Matrix4x4(const aiMatrix4x4* aMatrix)
{
	arr[0][0] = aMatrix->a1;
	arr[0][1] = aMatrix->a2;
	arr[0][2] = aMatrix->a3;
	arr[0][3] = aMatrix->a4;
	arr[1][0] = aMatrix->b1;
	arr[1][1] = aMatrix->b2;
	arr[1][2] = aMatrix->b3;
	arr[1][3] = aMatrix->b4;
	arr[2][0] = aMatrix->c1;
	arr[2][1] = aMatrix->c2;
	arr[2][2] = aMatrix->c3;
	arr[2][3] = aMatrix->c4;
	arr[3][0] = aMatrix->d1;
	arr[3][1] = aMatrix->d2;
	arr[3][2] = aMatrix->d3;
	arr[3][3] = aMatrix->d4;
}
