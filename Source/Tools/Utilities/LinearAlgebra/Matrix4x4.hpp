#define dim4x4 4
#pragma once 
#include "Vectors.hpp" 
#include <cmath> 
#include <DirectXMath.h>
#include <assimp/matrix4x4.h>

template <class T>
class Matrix4x4
{
public:
	// Creates the identity matrix.
	Matrix4x4<T>();
	// Copy Constructor.
	Matrix4x4<T>(const  Matrix4x4<T>& aMatrix);
	Matrix4x4<T>(const  Vector4<Vector4<T>> aVector);
	Matrix4x4(const DirectX::XMMATRIX& aMatrix);
	Matrix4x4(const aiMatrix4x4& aMatrix);
	// () operator for accessing element (row, column) for read/write or read,respectively.
	T& operator()(const int aRow,const int aColumn);
	const T& operator()(const int aRow,const int aColumn) const;

	// Static functions for creating rotation matrices.
	static Matrix4x4<T> CreateRotationAroundX(const T aAngleInRadians);
	static Matrix4x4<T> CreateRotationAroundY(const T aAngleInRadians);
	static Matrix4x4<T> CreateRotationAroundZ(const T aAngleInRadians);
	static Matrix4x4<T> CreateRotationMatrix(const Vector3<T>& aAnglesInRadians);
	static Matrix4x4<T> CreateScaleMatrix(const Vector3<T>& aScale);
	static Matrix4x4<T> CreateTranslationMatrix(const Vector3<T> aPosition);

	// Static function for creating a transpose of a matrix.
	static Matrix4x4<T> Transpose(const Matrix4x4<T>& aMatrixToTranspose);
	static Matrix4x4<T> GetFastInverse(const Matrix4x4<T>& aTransform);
	static Vector3<T> ReadPosition(const Matrix4x4<T>& aMatrix);

	static Matrix4x4<T> LookAt(const Vector3<T>& aFrom,const Vector3<T>& aTarget,const Vector3<T>& anUp);
	static Matrix4x4<T> CreateOrthographicProjection(float aLeftPlane,float aRightPlane,float aBottomPlane,float aTopPlane,float aNearPlane,float aFarPlane);
	void SetFromRaw(const T arr[16]);
private:
	T arr[4][4];
};
template<class T>
void Matrix4x4<T>::SetFromRaw(const T input[16])
{
	for(int i = 0; i < 16; i++)
	{
		arr[static_cast<int>(std::floor(i / 4))][i % 4] = input[i];
	}
}

template<class T>
inline Matrix4x4<T>::Matrix4x4(const Vector4<Vector4<T>> aVector)
{
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			arr[i][j] = aVector[i][j];
		}
	}
}

template<>
inline Matrix4x4<float>::Matrix4x4(const DirectX::XMMATRIX& aMatrix)
{
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			arr[i][j] = aMatrix.r[i].m128_f32[j];
		}
	}
}


template<>
inline Matrix4x4<float>::Matrix4x4(const aiMatrix4x4& aMatrix)
{
	//convert aiMatrix to standard matrix
	arr[0][0] = aMatrix.a1;
	arr[0][1] = aMatrix.a2;
	arr[0][2] = aMatrix.a3;
	arr[0][3] = aMatrix.a4;
	arr[1][0] = aMatrix.b1;
	arr[1][1] = aMatrix.b2;
	arr[1][2] = aMatrix.b3;
	arr[1][3] = aMatrix.b4;
	arr[2][0] = aMatrix.c1;
	arr[2][1] = aMatrix.c2;
	arr[2][2] = aMatrix.c3;
	arr[2][3] = aMatrix.c4;
	arr[3][0] = aMatrix.d1;
	arr[3][1] = aMatrix.d2;
	arr[3][2] = aMatrix.d3;
	arr[3][3] = aMatrix.d4;
}
template<class T>
Matrix4x4<T> Matrix4x4<T>::CreateRotationMatrix(const Vector3<T>& aAnglesInRadians)
{
	const Matrix4x4<T> xmatrix = Matrix4x4<T>::CreateRotationAroundX(aAnglesInRadians.x);
	const Matrix4x4<T> ymatrix = Matrix4x4<T>::CreateRotationAroundY(aAnglesInRadians.y);
	const Matrix4x4<T> zmatrix = Matrix4x4<T>::CreateRotationAroundZ(aAnglesInRadians.z);

	return xmatrix * ymatrix * zmatrix;
}

template <class T>
Matrix4x4<T> Matrix4x4<T>::CreateScaleMatrix(const Vector3<T>& aScale)
{
	Matrix4x4<T> matrix;
	matrix(1,1) = aScale.x;
	matrix(2,2) = aScale.y;
	matrix(3,3) = aScale.z;
	return matrix;
}


template<class T>
inline Matrix4x4<T>::Matrix4x4()
{
	for(short i = 1; i <= dim4x4; i++)
	{
		for(short j = 1; j <= dim4x4; j++)
		{
			if(i == j)
			{
				arr[i - 1][j - 1] = 1;
			}
			else
			{
				arr[i - 1][j - 1] = 0;
			}
		}
	}
}
template<class T>
inline Matrix4x4<T>::Matrix4x4(const Matrix4x4<T>& aMatrix)
{
	for(short i = 1; i <= dim4x4; i++)
	{
		for(short j = 1; j <= dim4x4; j++)
		{
			arr[i - 1][j - 1] = aMatrix(i,j);
		}
	}
}
#pragma region Operators
template <class T>
inline T& Matrix4x4<T>::operator()(const int aRow,const int aColumn)
{
	return arr[aRow - 1][aColumn - 1];
}
template <class T>
inline const T& Matrix4x4<T>::operator()(const int aRow,const int aColumn) const
{
	return arr[aRow - 1][aColumn - 1];
}

template <class T>	Matrix4x4<T> operator-(const Matrix4x4<T>& aMat1,const Matrix4x4<T>& aMat2)
{
	Matrix4x4<T> output;
	for(short i = 1; i <= dim4x4; i++)
	{
		for(short j = 1; j <= dim4x4; j++)
		{
			output(i,j) = aMat1(i,j) - aMat2(i,j);
		}
	}
	return output;
}
template <class T>	Matrix4x4<T> operator+(const Matrix4x4<T>& aMat1,const Matrix4x4<T>& aMat2)
{
	Matrix4x4<T> output;
	for(short i = 1; i <= dim4x4; i++)
	{
		for(short j = 1; j <= dim4x4; j++)
		{
			output(i,j) = aMat1(i,j) + aMat2(i,j);
		}
	}
	return output;
}
template <class T>	Matrix4x4<T> operator*(const Matrix4x4<T>& aMat1,const Matrix4x4<T>& aMat2)
{
	Matrix4x4<T> output;
	for(short i = 1; i <= dim4x4; i++)
	{
		for(short j = 1; j <= dim4x4; j++)
		{
			Vector4<T> RowMat1;
			Vector4<T> CollumnMat2;
			for(short k = 1; k <= dim4x4; k++)
			{
				RowMat1[k - 1] = aMat1(i,k);
				CollumnMat2[k - 1] = aMat2(k,j);
			}
			output(i,j) = RowMat1.Dot(CollumnMat2);
		}
	}
	return output;
}
template <class T>	Vector4<T>   operator*(const Vector4<T>& aVector,const Matrix4x4<T>& aMat2)
{
	Vector4<T> output;
	for(short j = 1; j <= dim4x4; j++)
	{
		T RowMat1 = 0;
		for(int k = 1; k <= dim4x4; k++)
		{
			T var = aMat2(k,j) * aVector[k - 1];
			RowMat1 += var;
		}
		output[j - 1] = RowMat1;
	}
	return output;
}
template <class T>	void operator+=(Matrix4x4<T>& aMat1,const Matrix4x4<T>& aMat2)
{
	for(short row = 1; row <= dim4x4; row++)
	{
		for(short collumn = 1; collumn <= dim4x4; collumn++)
		{
			aMat1(row,collumn) = aMat1(row,collumn) + aMat2(row,collumn);
		}
	}
}
template <class T>	void operator-=(Matrix4x4<T>& aMat1,const Matrix4x4<T>& aMat2)
{
	for(short row = 1; row <= dim4x4; row++)
	{
		for(short collumn = 1; collumn <= dim4x4; collumn++)
		{
			aMat1(row,collumn) = aMat1(row,collumn) - aMat2(row,collumn);
		}
	}
}
template <class T>	void operator*=(Matrix4x4<T>& aMat1,const Matrix4x4<T>& aMat2)
{
	Matrix4x4<T> output;
	for(short i = 1; i <= dim4x4; i++)
	{
		for(short j = 1; j <= dim4x4; j++)
		{
			Vector4<T> RowMat1;
			Vector4<T> CollumnMat2;
			for(short k = 1; k <= dim4x4; k++)
			{
				RowMat1[k - 1] = aMat1(i,k);
				CollumnMat2[k - 1] = aMat2(k,j);
			}
			output(i,j) = RowMat1.Dot(CollumnMat2);
		}
	}
	aMat1 = output;
}
template <class T>	bool operator==(const Matrix4x4<T>& aMat1,const Matrix4x4<T>& aMat2)
{
	for(short i = 1; i <= dim4x4; i++)
	{
		for(short j = 1; j <= dim4x4; j++)
		{
			if(aMat1(i,j) != aMat2(i,j))
			{
				return false;
			}
		}
	}
	return true;
}
template <class T>	bool operator!=(const Matrix4x4<T>& aMat1,const Matrix4x4<T>& aMat2)
{
	return !(aMat1 == aMat2);
}
#pragma endregion
template<class T>
inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundX(const T aAngleInRadians)
{
	Matrix4x4<T> output;

	output(2,2) = cos(aAngleInRadians);
	output(2,3) = sin(aAngleInRadians);
	output(3,2) = -sin(aAngleInRadians);
	output(3,3) = cos(aAngleInRadians);

	return output;
}
template<class T>
inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundY(const T aAngleInRadians)
{
	Matrix4x4<T> output;

	output(1,1) = cos(aAngleInRadians);
	output(3,1) = sin(aAngleInRadians);
	output(1,3) = -sin(aAngleInRadians);
	output(3,3) = cos(aAngleInRadians);

	return output;
}
template<class T>
inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundZ(const T aAngleInRadians)
{
	Matrix4x4<T> output;

	output(1,1) = cos(aAngleInRadians);
	output(2,1) = -sin(aAngleInRadians);
	output(1,2) = sin(aAngleInRadians);
	output(2,2) = cos(aAngleInRadians);

	return output;
}
template<class T>
inline Matrix4x4<T> Matrix4x4<T>::CreateTranslationMatrix(const Vector3<T> aPosition)
{
	Matrix4x4<T> output;

	output(4,1) = aPosition.x;
	output(4,2) = aPosition.y;
	output(4,3) = aPosition.z;

	return output;
}
template<class T>
inline Matrix4x4<T> Matrix4x4<T>::Transpose(const Matrix4x4<T>& aMatrixToTranspose)
{
	Matrix4x4<T> output;

	for(short i = 1; i <= dim4x4; i++)
	{
		for(short j = 1; j <= dim4x4; j++)
		{
			output(j,i) = aMatrixToTranspose(i,j);
		}
	}

	return output;
}
template<class T>
inline Matrix4x4<T> Matrix4x4<T>::GetFastInverse(const Matrix4x4<T>& aTransform)
{
	Matrix4x4<T> Rotation = aTransform;
	Matrix4x4<T> Transform;
	for(short i = 1; i <= 3; i++)
	{
		Rotation(4,i) = 0;
		Transform(4,i) = -aTransform(4,i);
	}
	Rotation = Rotation.Transpose(Rotation);

	return Transform * Rotation;  //Version 1 no scaling
}
template<class T>
inline Vector3<T> Matrix4x4<T>::ReadPosition(const Matrix4x4<T>& aMatrix)
{
	return Vector3<T>(aMatrix.arr[3][0],aMatrix.arr[3][1],aMatrix.arr[3][2]);
}
template<class T>
inline Matrix4x4<T> Matrix4x4<T>::LookAt(const Vector3<T>& aFrom,const Vector3<T>& aTarget,const Vector3<T>& anUp)
{
	Matrix4x4<T> result;

	Vector3<T> forward = (aTarget - aFrom).GetNormalized();
	Vector3<T> right = anUp.Cross(forward).GetNormalized();
	Vector3<T> up = forward.Cross(right);

	result(1,1) = right.x;
	result(1,2) = right.y;
	result(1,3) = right.z;

	result(2,1) = up.x;
	result(2,2) = up.y;
	result(2,3) = up.z;

	result(3,1) = forward.x;
	result(3,2) = forward.y;
	result(3,3) = forward.z;

	result(4,1) = aFrom.x;
	result(4,2) = aFrom.y;
	result(4,3) = aFrom.z;

	return result;
}

template<typename T>
inline Matrix4x4<T> Matrix4x4<T>::CreateOrthographicProjection(float aLeftPlane,float aRightPlane,float aBottomPlane,float aTopPlane,
	float aNearPlane,float aFarPlane)
{
	const float reciprocalWidth = 1.0f / (aRightPlane - aLeftPlane);
	const float reciprocalHeight = 1.0f / (aTopPlane - aBottomPlane);
	const float fRange = 1.0f / (aFarPlane - aNearPlane);

	Matrix4x4<T> result;
	result(1,1) = reciprocalWidth + reciprocalWidth;
	result(1,2) = 0.0f;
	result(1,3) = 0.0f;
	result(1,4) = 0.0f;

	result(2,1) = 0.0f;
	result(2,2) = reciprocalHeight + reciprocalHeight;
	result(2,3) = 0.0f;
	result(2,4) = 0.0f;

	result(3,1) = 0.0f;
	result(3,2) = 0.0f;
	result(3,3) = fRange;
	result(3,4) = 0.0f;

	result(4,1) = -(aLeftPlane + aRightPlane) * reciprocalWidth;
	result(4,2) = -(aTopPlane + aBottomPlane) * reciprocalHeight;
	result(4,3) = -fRange * aNearPlane;
	result(4,4) = 1.0f;


	return result;
} 
using Vector3f = Vector3<float>;
using Matrix = Matrix4x4<float>;