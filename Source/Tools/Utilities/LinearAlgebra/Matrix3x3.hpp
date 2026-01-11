#pragma once
#define dim3x3 3
#include "Matrix4x4.h"
#include "Vector3.hpp"
#include <ranges>

template <class T>
class Matrix3x3
{
public:
	// Creates the identity matrix.
	Matrix3x3<T>();
	Matrix3x3(const T* aMatrix);
	Matrix3x3(const std::initializer_list<T>& list);
	Matrix3x3(const T arr[3][3]);
	Matrix3x3(const Vector3<T> a, const Vector3<T> b, const Vector3<T> c);

	// Copy Constructor.
	Matrix3x3<T>(const Matrix3x3<T>& aMatrix);
	// Copies the top left 3x3 part of the Matrix4x4.
	Matrix3x3<T>(const Matrix4x4<T>& aMatrix);
	// () operator for accessing element (row, column) for read/write or read,respectively.
	T& operator()(int aRow, int aColumn);
	const T& operator()(int aRow, int aColumn) const;
	Matrix3x3<T>& operator=(const Matrix3x3<T>& aMatrix3x3) = default;

	~Matrix3x3<T>() = default;


	// Static functions for creating rotation matrices.
	static Matrix3x3<T> CreateRotationAroundX(T aAngleInRadians);
	static Matrix3x3<T> CreateRotationAroundY(T aAngleInRadians);
	static Matrix3x3<T> CreateRotationAroundZ(T aAngleInRadians);
	// Static function for creating a transpose of a matrix.
	static Matrix3x3<T> Transpose(const Matrix3x3<T>& aMatrixToTranspose);


	void Transpose();
	Matrix3x3<T> GetTranspose();

private:
	Vector3<T> r0;
	Vector3<T> r1;
	Vector3<T> r2;
};

template <class T>
Matrix3x3<T>::Matrix3x3()
{
	r0.x = 1;
	r1.y = 1;
	r2.z = 1;
}
template<class T>
inline Matrix3x3<T>::Matrix3x3(const Vector3<T> a, const Vector3<T> b, const Vector3<T> c)
{
	r0 = a;
	r1 = b;
	r2 = c;

}

template <class T>
Matrix3x3<T>::Matrix3x3(const T* aMatrix)
{
	r0.x = aMatrix[0]; r0.y = aMatrix[1]; r0.z = aMatrix[2];
	r1.x = aMatrix[3]; r1.y = aMatrix[4]; r1.z = aMatrix[5];
	r2.x = aMatrix[6]; r2.y = aMatrix[7]; r2.z = aMatrix[8];
}

template <class T>
Matrix3x3<T>::Matrix3x3(const std::initializer_list<T>& list) : Matrix3x3()
{
	r0.x = *(list.begin() + 0);
	r0.y = *(list.begin() + 1);
	r0.z = *(list.begin() + 2);

	r1.x = *(list.begin() + 0 + 3);
	r1.y = *(list.begin() + 1 + 3);
	r1.z = *(list.begin() + 2 + 3);

	r2.x = *(list.begin() + 0 + 6);
	r2.y = *(list.begin() + 1 + 6);
	r2.z = *(list.begin() + 2 + 6);
}

template <class T>
Matrix3x3<T>::Matrix3x3(const Matrix3x3<T>& aMatrix)
{
	r0 = aMatrix.r0;
	r1 = aMatrix.r1;
	r2 = aMatrix.r2;
}

template <class T>
Matrix3x3<T>::Matrix3x3(const Matrix4x4<T>& aMatrix)
{
	r0.x = aMatrix(1, 1);
	r0.y = aMatrix(1, 2);
	r0.z = aMatrix(1, 3);

	r1.x = aMatrix(2, 1);
	r1.y = aMatrix(2, 2);
	r1.z = aMatrix(2, 3);

	r2.x = aMatrix(3, 1);
	r2.y = aMatrix(3, 2);
	r2.z = aMatrix(3, 3);
}

#pragma region Operators
template <class T>
T& Matrix3x3<T>::operator()(const int aRow, const int aColumn)
{
	assert(aRow >= 1 && aRow <= 3);
	assert(aColumn >= 1 && aColumn <= 3);
	switch (aRow)
	{
	case 1:
		return r0[aColumn - 1];
	case 2:
		return r1[aColumn - 1];
	case 3:
		return r2[aColumn - 1];

	default:
		throw "Out of bound";
	}
}

template <class T>
const T& Matrix3x3<T>::operator()(const int aRow, const int aColumn) const
{
	assert(aRow >= 1 && aRow <= 3);
	assert(aColumn >= 1 && aColumn <= 3);

	switch (aRow)
	{
	case 1:
		return r0[aColumn - 1];
	case 2:
		return r1[aColumn - 1];
	case 3:
		return r2[aColumn - 1];

	default:
		throw "Out of bound";
	}
}

template <class T>
Matrix3x3<T> operator-(const Matrix3x3<T>& aMat1, const Matrix3x3<T>& aMat2)
{
	Matrix3x3<T> output;
	for (short row = 1; row <= dim3x3; row++)
	{
		for (short collumn = 1; collumn <= dim3x3; collumn++)
		{
			output(row, collumn) = aMat1(row, collumn) - aMat2(row, collumn);
		}
	}
	return output;
}

template <class T>
Matrix3x3<T> operator+(const Matrix3x3<T>& aMat1, const Matrix3x3<T>& aMat2)
{
	Matrix3x3<T> output;
	for (short row = 1; row <= dim3x3; row++)
	{
		for (short collumn = 1; collumn <= dim3x3; collumn++)
		{
			output(row, collumn) = aMat1(row, collumn) + aMat2(row, collumn);
		}
	}
	return output;
}

template <class T>
Matrix3x3<T> operator*(const Matrix3x3<T>& aMat1, const Matrix3x3<T>& aMat2)
{
	Matrix3x3<T> output;
	for (short i = 1; i <= dim3x3; i++)
	{
		for (short j = 1; j <= dim3x3; j++)
		{
			Vector3<T> RowMat1;
			Vector3<T> CollumnMat2;
			for (short k = 1; k <= dim3x3; k++)
			{
				RowMat1[k - 1] = aMat1(i, k);
				CollumnMat2[k - 1] = aMat2(k, j);
			}
			output(i, j) = RowMat1.Dot(CollumnMat2);
		}
	}
	return output;
}

template <class T>
Vector3<T> operator*(const Vector3<T>& aVector, const Matrix3x3<T>& aMat2)
{
	Vector3<T> output;
	for (short j = 1; j <= dim3x3; j++)
	{
		T RowMat1 = 0;
		for (int k = 1; k <= dim3x3; k++)
		{
			T var = aMat2(k, j) * aVector[k - 1];
			RowMat1 += var;
		}
		output[j - 1] = RowMat1;
	}
	return output;
}

template <class T>
void operator+=(Matrix3x3<T>& aMat1, const Matrix3x3<T>& aMat2)
{
	for (short row = 1; row <= dim3x3; row++)
	{
		for (short collumn = 1; collumn <= dim3x3; collumn++)
		{
			aMat1(row, collumn) = aMat1(row, collumn) + aMat2(row, collumn);
		}
	}
}

template <class T>
void operator-=(Matrix3x3<T>& aMat1, const Matrix3x3<T>& aMat2)
{
	for (short row = 1; row <= dim3x3; row++)
	{
		for (short collumn = 1; collumn <= dim3x3; collumn++)
		{
			aMat1(row, collumn) = aMat1(row, collumn) - aMat2(row, collumn);
		}
	}
}

template <class T>
void operator*=(Matrix3x3<T>& aMat1, const Matrix3x3<T>& aMat2)
{
	Matrix3x3<T> output;
	for (short i = 1; i <= dim3x3; i++)
	{
		for (short j = 1; j <= dim3x3; j++)
		{
			Vector4<T> RowMat1;
			Vector4<T> CollumnMat2;
			for (short k = 1; k <= dim3x3; k++)
			{
				RowMat1[k - 1] = aMat1(i, k);
				CollumnMat2[k - 1] = aMat2(k, j);
			}
			output(i, j) = RowMat1.Dot(CollumnMat2);
		}
	}
	aMat1 = output;
}

template <class T>
bool operator==(const Matrix3x3<T>& aMat1, const Matrix3x3<T>& aMat2)
{
	for (short i = 1; i <= dim3x3; i++)
	{
		for (short j = 1; j <= dim3x3; j++)
		{
			if (aMat1(i, j) != aMat2(i, j))
			{
				return false;
			}
		}
	}
	return true;
}

template <class T>
bool operator!=(const Matrix3x3<T>& aMat1, const Matrix3x3<T>& aMat2)
{
	return !(aMat1 == aMat2);
}
#pragma endregion
template <class T>
Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundX(const T aAngleInRadians)
{
	Matrix3x3<T> output;

	output(2, 2) = cos(aAngleInRadians);
	output(2, 3) = sin(aAngleInRadians);
	output(3, 2) = -sin(aAngleInRadians);
	output(3, 3) = cos(aAngleInRadians);

	return output;
}

template <class T>
Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundY(const T aAngleInRadians)
{
	Matrix3x3<T> output;

	output(1, 1) = cos(aAngleInRadians);
	output(3, 1) = sin(aAngleInRadians);
	output(1, 3) = -sin(aAngleInRadians);
	output(3, 3) = cos(aAngleInRadians);

	return output;
}

template <class T>
Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundZ(const T aAngleInRadians)
{
	Matrix3x3<T> output;

	output(1, 1) = cos(aAngleInRadians);
	output(2, 1) = -sin(aAngleInRadians);
	output(1, 2) = sin(aAngleInRadians);
	output(2, 2) = cos(aAngleInRadians);

	return output;
}

template <class T>
Matrix3x3<T> Matrix3x3<T>::Transpose(const Matrix3x3<T>& aMatrixToTranspose)
{
	Matrix3x3<T> output;

	for (short i = 1; i <= dim3x3; i++)
	{
		for (short j = 1; j <= dim3x3; j++)
		{
			output(j, i) = aMatrixToTranspose(i, j);
		}
	}

	return output;
}

template<class T>
inline void Matrix3x3<T>::Transpose() { *this = Transpose(*this); }

template<class T>
inline Matrix3x3<T> Matrix3x3<T>::GetTranspose()
{
	return Transpose(*this);
}
