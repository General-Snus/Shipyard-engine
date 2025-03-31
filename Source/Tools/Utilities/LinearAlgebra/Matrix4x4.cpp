#include "Matrix4x4.h"
#include "External/Assimp/matrix4x4.h"
#include <DirectXMath.h>

template <> Matrix4x4<float>::Matrix4x4(const DirectX::XMMATRIX *aMatrix)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            operator()(i + 1, j + 1) = aMatrix->r[i].m128_f32[j];
        }
    }
}

template <> Matrix4x4<float>::Matrix4x4(const aiMatrix4x4 *aMatrix)
{
    arr[0] = aMatrix->a1;
    arr[1] = aMatrix->a2;
    arr[2] = aMatrix->a3;
    arr[3] = aMatrix->a4;
    arr[4] = aMatrix->b1;
    arr[5] = aMatrix->b2;
    arr[6] = aMatrix->b3;
    arr[7] = aMatrix->b4;
    arr[8] = aMatrix->c1;
    arr[9] = aMatrix->c2;
    arr[10] = aMatrix->c3;
    arr[11] = aMatrix->c4;
    arr[12] = aMatrix->d1;
    arr[13] = aMatrix->d2;
    arr[14] = aMatrix->d3;
    arr[15] = aMatrix->d4;
}
