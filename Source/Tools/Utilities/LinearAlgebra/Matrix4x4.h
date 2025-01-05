#pragma once
#include <cmath>
#include <array>
#include "Vectors.hpp"  

#define DIM4_X4 4
namespace DirectX {
	struct XMMATRIX;
}

template <typename Type>
class aiMatrix4x4t;
using aiMatrix4x4 = aiMatrix4x4t<float>;

template <class T = float>
class Matrix4x4 {
public:
	// Creates the identity matrix.
	Matrix4x4<T>();
	// Copy Constructor.
	Matrix4x4<T>(const Matrix4x4<T>& aMatrix);
	Matrix4x4<T>(Vector4<Vector4<T>> aVector);
	explicit Matrix4x4(const DirectX::XMMATRIX* aMatrix);
	explicit Matrix4x4(const aiMatrix4x4* aMatrix);

	T& operator()(int aRow,int aColumn);
	const T& operator()(int aRow,int aColumn) const;

	T* operator&() {
		return arr.data();
	}

	const T* operator&() const {
		return arr.data();
	}

	// Static functions for creating rotation matrices.
	static Matrix4x4<T> CreateRotationAroundX(T aAngleInRadians);
	static Matrix4x4<T> CreateRotationAroundY(T aAngleInRadians);
	static Matrix4x4<T> CreateRotationAroundZ(T aAngleInRadians);
	static Matrix4x4<T> CreateRotationMatrix(const Vector3<T>& aAnglesInRadians);
	static Matrix4x4<T> CreateScaleMatrix(const Vector3<T>& aScale);
	static Matrix4x4<T> CreateTranslationMatrix(Vector3<T> aPosition);
	static Matrix4x4<T> Transpose(const Matrix4x4<T>& aMatrixToTranspose);
	static Matrix4x4<T> GetFastInverse(const Matrix4x4<T>& aTransform);
	static Matrix4x4<T> GetFastInverse(const Matrix4x4<T>& aTransform,const Vector3<T>& scaling);
	static T GetDeterminant(const Matrix4x4<T>& aTransform);
	static Matrix4x4<T> GetAdjugate(const Matrix4x4<T>& aTransform);
	static Matrix4x4<T> Invert(const Matrix4x4<T>& aTransform);
	static T magnitudeOfRow(const Matrix4x4<T>& aMatrix,const int row);
	static Matrix4x4<T> rotationMatrix(const Matrix4x4<T>& aMatrix);

	static Vector3<T> scale(const Matrix4x4<T>& aMatrix);
	static Vector3<T> position(const Matrix4x4<T>& aMatrix);

	static Matrix4x4<T> LookAt(const Vector3<T>& aFrom,const Vector3<T>& aTarget,const Vector3<T>& anUp);
	static Matrix4x4<T> CreateOrthographicProjection(float aLeftPlane,float aRightPlane,float aBottomPlane,
		float aTopPlane,float  aNearPlane,float  aFarPlane);


	void SetFromRaw(const T arr[16]);
	void         Transpose();
	void         FastInverse();
	T GetDeterminant() const;
	Matrix4x4<T> GetAdjugate() const;
	Matrix4x4<T> GetTranspose() const;
	Matrix4x4<T> GetFastInverse() const;
	Vector3<T> scale() const;
	Vector3<T> position() const;
	Matrix4x4<T> rotationMatrix() const ;
	T magnitudeOfRow(const int row) const;

	// STRANGER DANGER
	T* GetMatrixPtr();

private:
	std::array<T,16> arr;
};

template <class T>
void Matrix4x4<T>::SetFromRaw(const T input[16]) {
	arr = input;
}

template <class T>
Matrix4x4<T>::Matrix4x4(const Vector4<Vector4<T>> aVector) {
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			this->operator()(i + 1,j + 1) = aVector[i][j];
		}
	}
}

template <class T>
Matrix4x4<T> Matrix4x4<T>::CreateRotationMatrix(const Vector3<T>& aAnglesInRadians) {
	const Matrix4x4<T> xmatrix = Matrix4x4<T>::CreateRotationAroundX(aAnglesInRadians.x);
	const Matrix4x4<T> ymatrix = Matrix4x4<T>::CreateRotationAroundY(aAnglesInRadians.y);
	const Matrix4x4<T> zmatrix = Matrix4x4<T>::CreateRotationAroundZ(aAnglesInRadians.z);

	return xmatrix * ymatrix * zmatrix;
}

template <class T>
Matrix4x4<T> Matrix4x4<T>::CreateScaleMatrix(const Vector3<T>& aScale) {
	Matrix4x4<T> matrix;
	matrix(1,1) = aScale.x;
	matrix(2,2) = aScale.y;
	matrix(3,3) = aScale.z;
	return matrix;
}

template <class T>
Matrix4x4<T>::Matrix4x4() {
	arr[0] = T(1);
	arr[1] = T(0);
	arr[2] = T(0);
	arr[3] = T(0);

	arr[4] = T(0);
	arr[5] = T(1);
	arr[6] = T(0);
	arr[7] = T(0);

	arr[8] = T(0);
	arr[9] = T(0);
	arr[10] = T(1);
	arr[11] = T(0);

	arr[12] = T(0);
	arr[13] = T(0);
	arr[14] = T(0);
	arr[15] = T(1);
}

template <class T>
Matrix4x4<T>::Matrix4x4(const Matrix4x4<T>& aMatrix) : arr(aMatrix.arr) {

}
#pragma region Operators
template <class T>
T& Matrix4x4<T>::operator()(const int aRow,const int aColumn) {
	return arr[4 * (aRow - 1) + aColumn - 1];
}

template <class T>
const T& Matrix4x4<T>::operator()(const int aRow,const int aColumn) const {
	return arr[4 * (aRow - 1) + aColumn - 1];
}

template <class T>
Matrix4x4<T> operator-(const Matrix4x4<T>& aMat1,const Matrix4x4<T>& aMat2) {
	Matrix4x4<T> output;
	for(short i = 1; i <= DIM4_X4; i++) {
		for(short j = 1; j <= DIM4_X4; j++) {
			output(i,j) = aMat1(i,j) - aMat2(i,j);
		}
	}
	return output;
}

template <class T>
Matrix4x4<T> operator+(const Matrix4x4<T>& aMat1,const Matrix4x4<T>& aMat2) {
	Matrix4x4<T> output;
	for(short i = 1; i <= DIM4_X4; i++) {
		for(short j = 1; j <= DIM4_X4; j++) {
			output(i,j) = aMat1(i,j) + aMat2(i,j);
		}
	}
	return output;
}

template <class T>
Matrix4x4<T> operator*(const Matrix4x4<T>& aMat1,const Matrix4x4<T>& aMat2) {
	Matrix4x4<T> output;
	for(short i = 1; i <= DIM4_X4; i++) {
		for(short j = 1; j <= DIM4_X4; j++) {
			Vector4<T> RowMat1;
			Vector4<T> CollumnMat2;
			for(short k = 1; k <= DIM4_X4; k++) {
				RowMat1[k - 1] = aMat1(i,k);
				CollumnMat2[k - 1] = aMat2(k,j);
			}
			output(i,j) = RowMat1.Dot(CollumnMat2);
		}
	}
	return output;
}

template <class T>
Vector4<T> operator*(const Vector4<T>& aVector,const Matrix4x4<T>& aMat2) {
	Vector4<T> output;
	for(short i = 1; i <= DIM4_X4; i++) {
		T RowMat1 = 0;
		for(int j = 1; j <= DIM4_X4; j++) {
			T var = aMat2(j,i) * aVector[j - 1];
			RowMat1 += var;
		}
		output[i - 1] = RowMat1;
	}
	return output;
}

template <class T>
Vector4<T> operator*(const Matrix4x4<T>& aMat2,const Vector4<T>& aVector) {
	Vector4<T> output;
	for(short i = 1; i <= DIM4_X4; i++) {
		T RowMat1 = 0;
		for(int j = 1; j <= DIM4_X4; j++) {
			T var = aMat2(i,j) * aVector[j - 1];
			RowMat1 += var;
		}
		output[i - 1] = RowMat1;
	}
	return output;
}


template <typename T>
Matrix4x4<T> operator*(T scalar,const Matrix4x4<T>& matrix) {
	return matrix * scalar;
}

template <typename T>
Matrix4x4<T> operator*(const Matrix4x4<T>& matrix,T scalar) {
	Matrix4x4<T> result;
	for(int row = 1; row <= 4; ++row) {
		for(int col = 1; col <= 4; ++col) {
			result(row,col) = matrix(row,col) * scalar;
		}
	}
	return result;
}

template <class T>
void operator+=(Matrix4x4<T>& aMat1,const Matrix4x4<T>& aMat2) {
	for(short row = 1; row <= DIM4_X4; row++) {
		for(short collumn = 1; collumn <= DIM4_X4; collumn++) {
			aMat1(row,collumn) = aMat1(row,collumn) + aMat2(row,collumn);
		}
	}
}

template <class T>
void operator-=(Matrix4x4<T>& aMat1,const Matrix4x4<T>& aMat2) {
	for(short row = 1; row <= DIM4_X4; row++) {
		for(short collumn = 1; collumn <= DIM4_X4; collumn++) {
			aMat1(row,collumn) = aMat1(row,collumn) - aMat2(row,collumn);
		}
	}
}

template <class T>
void operator*=(Matrix4x4<T>& aMat1,const Matrix4x4<T>& aMat2) {
	Matrix4x4<T> output;
	for(short i = 1; i <= DIM4_X4; i++) {
		for(short j = 1; j <= DIM4_X4; j++) {
			Vector4<T> RowMat1;
			Vector4<T> CollumnMat2;
			for(short k = 1; k <= DIM4_X4; k++) {
				RowMat1[k - 1] = aMat1(i,k);
				CollumnMat2[k - 1] = aMat2(k,j);
			}
			output(i,j) = RowMat1.Dot(CollumnMat2);
		}
	}
	aMat1 = output;
}

template <class T>
bool operator==(const Matrix4x4<T>& aMat1,const Matrix4x4<T>& aMat2) {
	for(short i = 1; i <= DIM4_X4; i++) {
		for(short j = 1; j <= DIM4_X4; j++) {
			if(aMat1(i,j) != aMat2(i,j)) {
				return false;
			}
		}
	}
	return true;
}

template <class T>
bool operator!=(const Matrix4x4<T>& aMat1,const Matrix4x4<T>& aMat2) {
	return !(aMat1 == aMat2);
}
#pragma endregion


template <class T>
Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundX(const T aAngleInRadians) {
	Matrix4x4<T> output;

	output(2,2) = cos(aAngleInRadians);
	output(2,3) = sin(aAngleInRadians);
	output(3,2) = -sin(aAngleInRadians);
	output(3,3) = cos(aAngleInRadians);

	return output;
}

template <class T>
Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundY(const T aAngleInRadians) {
	Matrix4x4<T> output;

	output(1,1) = cos(aAngleInRadians);
	output(3,1) = sin(aAngleInRadians);
	output(1,3) = -sin(aAngleInRadians);
	output(3,3) = cos(aAngleInRadians);

	return output;
}

template <class T>
Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundZ(const T aAngleInRadians) {
	Matrix4x4<T> output;

	output(1,1) = cos(aAngleInRadians);
	output(2,1) = -sin(aAngleInRadians);
	output(1,2) = sin(aAngleInRadians);
	output(2,2) = cos(aAngleInRadians);

	return output;
}

template <class T>
Matrix4x4<T> Matrix4x4<T>::CreateTranslationMatrix(const Vector3<T> aPosition) {
	Matrix4x4<T> output;

	output(4,1) = aPosition.x;
	output(4,2) = aPosition.y;
	output(4,3) = aPosition.z;

	return output;
}

template <class T>
Matrix4x4<T> Matrix4x4<T>::Transpose(const Matrix4x4<T>& aMatrixToTranspose) {
	Matrix4x4<T> output;

	for(short i = 1; i <= DIM4_X4; i++) {
		for(short j = 1; j <= DIM4_X4; j++) {
			output(j,i) = aMatrixToTranspose(i,j);
		}
	}

	return output;
}

template <class T>
Matrix4x4<T> Matrix4x4<T>::GetFastInverse(const Matrix4x4<T>& aTransform) {
	Matrix4x4<T> Rotation = aTransform;
	Matrix4x4<T> Transform;
	for(short i = 1; i <= 3; i++) {
		Rotation(4,i) = 0;
		Transform(4,i) = -aTransform(4,i);
	}
	Rotation = Rotation.Transpose(Rotation);

	return Transform * Rotation; // Version 1 no scaling
}

template <class T>
Matrix4x4<T> Matrix4x4<T>::GetFastInverse(const Matrix4x4<T>& aTransform,const Vector3<T>& scaling) {
	Matrix4x4<T> Rotation = aTransform;
	Matrix4x4<T> Transform;
	for(short i = 1; i <= 3; i++) {
		Rotation(4,i) = 0;
		Transform(4,i) = -aTransform(4,i);
	}
	Rotation = Rotation.Transpose(Rotation);

	Matrix4x4<T> Scale;
	Scale(1,1) = std::pow(1 / scaling.x,2);
	Scale(2,2) = std::pow(1 / scaling.y,2);
	Scale(3,3) = std::pow(1 / scaling.z,2);

	return Transform * Rotation * Scale; // Version 1 no scaling
}

template<class T>
inline T Matrix4x4<T>::GetDeterminant(const Matrix4x4<T>& aTransform) {
	const auto& m = aTransform; // Alias for convenience

	// Determinant of a 3x3 submatrix
	auto det3x3 = [](T a,T b,T c,T d,T e,T f,T g,T h,T i) {
		return a * (e * i - f * h) - b * (d * i - f * g) + c * (d * h - e * g);
		};

	T detM11 = det3x3(m(2,2),m(2,3),m(2,4),
		m(3,2),m(3,3),m(3,4),
		m(4,2),m(4,3),m(4,4));

	T detM12 = det3x3(m(2,1),m(2,3),m(2,4),
		m(3,1),m(3,3),m(3,4),
		m(4,1),m(4,3),m(4,4));

	T detM13 = det3x3(m(2,1),m(2,2),m(2,4),
		m(3,1),m(3,2),m(3,4),
		m(4,1),m(4,2),m(4,4));

	T detM14 = det3x3(
		m(2,1),m(2,2),m(2,3),
		m(3,1),m(3,2),m(3,3),
		m(4,1),m(4,2),m(4,3)
	);

	return m(1,1) * detM11 - m(1,2) * detM12 + m(1,3) * detM13 - m(1,4) * detM14;
}

template <class T>
inline Matrix4x4<T> Matrix4x4<T>::GetAdjugate(const Matrix4x4<T>& m) {
	Matrix4x4<T> adj;

	// Helper lambda to compute the determinant of a 3x3 matrix
	auto det3x3 = [](T a,T b,T c,T d,T e,T f,T g,T h,T i) {
		return a * (e * i - f * h) - b * (d * i - f * g) + c * (d * h - e * g);
		};

	adj(1,1) = det3x3(m(2,2),m(2,3),m(2,4),m(3,2),m(3,3),m(3,4),m(4,2),m(4,3),m(4,4));
	adj(2,1) = -det3x3(m(2,1),m(2,3),m(2,4),m(3,1),m(3,3),m(3,4),m(4,1),m(4,3),m(4,4));
	adj(3,1) = det3x3(m(2,1),m(2,2),m(2,4),m(3,1),m(3,2),m(3,4),m(4,1),m(4,2),m(4,4));
	adj(4,1) = -det3x3(m(2,1),m(2,2),m(2,3),m(3,1),m(3,2),m(3,3),m(4,1),m(4,2),m(4,3));

	adj(1,2) = -det3x3(m(1,2),m(1,3),m(1,4),m(3,2),m(3,3),m(3,4),m(4,2),m(4,3),m(4,4));
	adj(2,2) = det3x3(m(1,1),m(1,3),m(1,4),m(3,1),m(3,3),m(3,4),m(4,1),m(4,3),m(4,4));
	adj(3,2) = -det3x3(m(1,1),m(1,2),m(1,4),m(3,1),m(3,2),m(3,4),m(4,1),m(4,2),m(4,4));
	adj(4,2) = det3x3(m(1,1),m(1,2),m(1,3),m(3,1),m(3,2),m(3,3),m(4,1),m(4,2),m(4,3));

	adj(1,3) = det3x3(m(1,2),m(1,3),m(1,4),m(2,2),m(2,3),m(2,4),m(4,2),m(4,3),m(4,4));
	adj(2,3) = -det3x3(m(1,1),m(1,3),m(1,4),m(2,1),m(2,3),m(2,4),m(4,1),m(4,3),m(4,4));
	adj(3,3) = det3x3(m(1,1),m(1,2),m(1,4),m(2,1),m(2,2),m(2,4),m(4,1),m(4,2),m(4,4));
	adj(4,3) = -det3x3(m(1,1),m(1,2),m(1,3),m(2,1),m(2,2),m(2,3),m(4,1),m(4,2),m(4,3));

	adj(1,4) = -det3x3(m(1,2),m(1,3),m(1,4),m(2,2),m(2,3),m(2,4),m(3,2),m(3,3),m(3,4));
	adj(2,4) = det3x3(m(1,1),m(1,3),m(1,4),m(2,1),m(2,3),m(2,4),m(3,1),m(3,3),m(3,4));
	adj(3,4) = -det3x3(m(1,1),m(1,2),m(1,4),m(2,1),m(2,2),m(2,4),m(3,1),m(3,2),m(3,4));
	adj(4,4) = det3x3(m(1,1),m(1,2),m(1,3),m(2,1),m(2,2),m(2,3),m(3,1),m(3,2),m(3,3));

	return adj;
}

template<class T>
inline Matrix4x4<T> Matrix4x4<T>::Invert(const Matrix4x4<T>& m) {
	const T det = GetDeterminant(m);
	if(det == 0) {
		throw std::runtime_error("Matrix is singular and cannot be inverted.");
	}
	return (1 / det) * GetAdjugate(m);
}


template<class T>
inline T Matrix4x4<T>::magnitudeOfRow(const Matrix4x4<T>& aMatrix,const int row) {
	return Vector4<T>(aMatrix(row + 1,1),aMatrix(row + 1,2),aMatrix(row + 1,3),aMatrix(row + 1,4)).Length();
}

template<class T>
inline Matrix4x4<T> Matrix4x4<T>::rotationMatrix(const Matrix4x4<T>& aMatrix) {
	const Vector3<T> scaling = aMatrix.scale();
	Matrix4x4<T> rotationMatrix = aMatrix;

	for(int i = 1; i < 4; i++) {
		rotationMatrix(i,1) /= scaling.x;
		rotationMatrix(i,2) /= scaling.y;
		rotationMatrix(i,3) /= scaling.z;  
	}

	rotationMatrix(4,1) = T(0);
	rotationMatrix(4,2) = T(0);
	rotationMatrix(4,3) = T(0);
	rotationMatrix(4,4) = T(1);

	return rotationMatrix;
}

template<class T>
inline Vector3<T> Matrix4x4<T>::scale(const Matrix4x4<T>& aMatrix) {
	return Vector3<T>(aMatrix.magnitudeOfRow(0),aMatrix.magnitudeOfRow(1),aMatrix.magnitudeOfRow(2));
}

template<class T>
inline Vector3<T> Matrix4x4<T>::position(const Matrix4x4<T>& aMatrix) {
	return Vector3<T>(aMatrix(4,1),aMatrix(4,2),aMatrix(4,3));
}

template <class T>
T* Matrix4x4<T>::GetMatrixPtr() {
	return arr.data();
}

template <class T>
void Matrix4x4<T>::Transpose() {
	arr = Matrix4x4<T>::Transpose(*this);
}

template <class T>
Matrix4x4<T> Matrix4x4<T>::GetTranspose() const {
	return Matrix4x4<T>::Transpose(*this);
}

template <class T>
void Matrix4x4<T>::FastInverse() {
	*this = Matrix4x4<T>::GetFastInverse(*this);
}

template<class T>
inline T Matrix4x4<T>::GetDeterminant() const {
	return Matrix4x4<T>::GetDeterminant(*this);
}

template<class T>
inline Matrix4x4<T> Matrix4x4<T>::GetAdjugate() const {
	return Matrix4x4<T>::GetAdjugate(*this);
}

template <class T>
Matrix4x4<T> Matrix4x4<T>::GetFastInverse() const {
	return Matrix4x4<T>::GetFastInverse(*this);
}

template<class T>
inline Vector3<T> Matrix4x4<T>::scale() const {
	return scale(*this);
}

template<class T>
inline Vector3<T> Matrix4x4<T>::position() const {
	return position(*this);
}

template<class T>
inline Matrix4x4<T> Matrix4x4<T>::rotationMatrix() const {
	return rotationMatrix(*this);
}

template<class T>
inline T Matrix4x4<T>::magnitudeOfRow(const int row) const {
	return magnitudeOfRow(*this,row);
}

template <class T>
Matrix4x4<T> Matrix4x4<T>::LookAt(const Vector3<T>& aFrom,const Vector3<T>& aTarget,const Vector3<T>& anUp) {
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

template <typename T>
Matrix4x4<T> Matrix4x4<T>::CreateOrthographicProjection(float aLeftPlane,float aRightPlane,float aBottomPlane,
	float aTopPlane,float  aNearPlane,float  aFarPlane) {
	Matrix4x4<T> result;
	const float  ViewWidth = std::abs(aRightPlane - aLeftPlane);
	const float  ViewHeight = std::abs(aTopPlane - aBottomPlane);
	const float  fRange = 1.0f / (aFarPlane - aNearPlane);

	result(1,1) = 2.0f / ViewWidth;
	result(1,2) = 0.0f;
	result(1,3) = 0.0f;
	result(1,4) = 0.0f;
	result(2,1) = 0.0f;
	result(2,2) = 2.0f / ViewHeight;
	result(2,3) = 0.0f;
	result(2,4) = 0.0f;
	result(3,1) = 0.0f;
	result(3,2) = 0.0f;
	result(3,3) = fRange;
	result(3,4) = 0.0f;
	result(4,1) = 0.0f;
	result(4,2) = 0.0f;
	result(4,3) = -fRange * aNearPlane;
	result(4,4) = 1.0f;
	return result;
}

using Vector3f = Vector3<float>;
using Matrix = Matrix4x4<float>;
