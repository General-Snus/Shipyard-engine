#pragma once
#define dim3x3 3
#include "Matrix4x4.h"
#include "Vector3.hpp"

template <class T>
class Matrix3x3
{
public:
	// Creates the identity matrix.
	Matrix3x3<T>();
	// Copy Constructor.
	Matrix3x3<T>(const Matrix3x3<T>& aMatrix);
	// Copies the top left 3x3 part of the Matrix4x4.
	Matrix3x3<T>(const Matrix4x4<T>& aMatrix);
	// () operator for accessing element (row, column) for read/write or read,respectively.
	T&            operator()(int aRow, int aColumn);
	const T&      operator()(int aRow, int aColumn) const;
	Matrix3x3<T>& operator=(const Matrix3x3<T>& aMatrix3x3) = default;

	~Matrix3x3<T>() = default;


	// Static functions for creating rotation matrices.
	static Matrix3x3<T> CreateRotationAroundX(T aAngleInRadians);
	static Matrix3x3<T> CreateRotationAroundY(T aAngleInRadians);
	static Matrix3x3<T> CreateRotationAroundZ(T aAngleInRadians);
	// Static function for creating a transpose of a matrix.
	static Matrix3x3<T> Transpose(const Matrix3x3<T>& aMatrixToTranspose);

private:
	T arr[dim3x3][dim3x3];
};

template <class T>
Matrix3x3<T>::Matrix3x3()
{
	for (short i = 1; i <= dim3x3; i++)
	{
		for (short j = 1; j <= dim3x3; j++)
		{
			if (i == j)
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

template <class T>
Matrix3x3<T>::Matrix3x3(const Matrix3x3<T>& aMatrix)
{
	for (short i = 1; i <= dim3x3; i++)
	{
		for (short j = 1; j <= dim3x3; j++)
		{
			arr[i - 1][j - 1] = aMatrix(i, j);
		}
	}
}

template <class T>
Matrix3x3<T>::Matrix3x3(const Matrix4x4<T>& aMatrix)
{
	for (short i = 1; i <= dim3x3; i++)
	{
		for (short j = 1; j <= dim3x3; j++)
		{
			arr[i - 1][j - 1] = aMatrix(i, j);
		}
	}
}
#pragma region Operators
template <class T>
T& Matrix3x3<T>::operator()(const int aRow, const int aColumn)
{
	return arr[aRow - 1][aColumn - 1];
}

template <class T>
const T& Matrix3x3<T>::operator()(const int aRow, const int aColumn) const
{
	return arr[aRow - 1][aColumn - 1];
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
