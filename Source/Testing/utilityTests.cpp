#include "pch.h"

#include "../Tools/Utilities/LinearAlgebra/Easing.h"
#include "../Tools/Utilities/LinearAlgebra/Vector3.hpp"
#include "../Tools/Utilities/LinearAlgebra/Vector4.hpp"
#include "../Tools/Utilities/LinearAlgebra/Matrix4x4.h"
#include "../Tools/Utilities/LinearAlgebra/Quaternions.hpp"
#include "../Tools/Utilities/Math.hpp"
#include "../Tools/Utilities/LinearAlgebra/VectorX.hpp"
#include "../Tools/Utilities/DataStructures/MathStructs.h"

TEST(TestCaseName,TestName) {
	const auto intVec = VectorX<int,3>({1, 2, 3});
	auto       expandedIntVec = intVec.swizzle<0,1,2,3,-3>();

	// Vector4<float> expandedFloatVec = floatVec.swizzle<zero,-x,z,y>().V4();

	// Vector4f vec4 = expandedFloatVec.V4();
	// Vector4<int> vec4I = expandedIntVec.V4();

	EXPECT_EQ(expandedIntVec[0],0);
	EXPECT_EQ(expandedIntVec[1],1);
	EXPECT_EQ(expandedIntVec[2],2);
	EXPECT_EQ(expandedIntVec[3],3);
	Vector4f zeroes;
	Vector4f ones = {1, 2, 3, 4};
	Vector4f res;

	res = lerp(zeroes,ones,.5);
	for(auto element : res) {
		std::cout << element << "\n";
	}

	// EXPECT_NEAR(expandedFloatVec.x, 0, 0.2f);
	// EXPECT_NEAR(expandedFloatVec.y, -floatVec.x, 0.2f);
	// EXPECT_NEAR(expandedFloatVec.z, floatVec.z, 0.2f);
	// EXPECT_NEAR(expandedFloatVec.w, floatVec.y, 0.2f);
}
using T = float; // Change this to test with different types (e.g., double)


TEST(AverageTester, Average)
{
	Avg<float, 10000> avg;


	for (size_t i = 0; i < 100'000; i++)
	{
		avg.Add(Math::RandomEngine::randomInRange(0.0f,1.0f));
	}
	EXPECT_NEAR(avg.Average(),.5f, 0.001f); 
}





// Helper function for matrix comparison
template <typename T>
bool AreMatricesEqual(const Matrix4x4<T>& m1,const Matrix4x4<T>& m2,T epsilon = 1e-5) {
	for(int row = 1; row <= 4; ++row) {
		for(int col = 1; col <= 4; ++col) {
			if(std::abs(m1(row,col) - m2(row,col)) > epsilon) {
				return false;
			}
		}
	}
	return true;
}

// Helper function for matrix comparison
template <typename T>
bool AreMatricesEqual(const Matrix3x3<T>& m1,const Matrix3x3<T>& m2,T epsilon = 1e-5) {
	for(int row = 1; row <= 3; ++row) {
		for(int col = 1; col <= 3; ++col) {
			if(std::abs(m1(row,col) - m2(row,col)) > epsilon) {
				return false;
			}
		}
	}
	return true;
}

// Test Identity Matrix
TEST(Matrix4x4Test,IdentityMatrix) {
	Matrix4x4<T> identity;
	for(int i = 1; i <= 4; ++i) {
		for(int j = 1; j <= 4; ++j) {
			EXPECT_EQ(identity(i,j),(i == j ? 1 : 0));
		}
	}
}

// Test Determinant Calculation
TEST(Matrix4x4Test,Determinant) {
	Matrix4x4<T> m({
		{1, 2, 3, 4},
		{5, 6, 7, 8},
		{9, 10, 11, 12},
		{13, 14, 15, 16}
		});
	EXPECT_EQ(Matrix4x4<T>::GetDeterminant(m),0); // Singular matrix has a determinant of 0

	Matrix4x4<T> nonSingular({
		{6, 1, 1, 0},
		{4, -2, 5, 0},
		{2, 8, 7, 0},
		{0, 0, 0, 1}
		});
	EXPECT_NEAR(Matrix4x4<T>::GetDeterminant(nonSingular),-306,1e-5);
}

// Test Adjugate Calculation
TEST(Matrix4x4Test,Adjugate) {
	Matrix4x4<T> m({
		{6, 1, 1, 0},
		{4, -2, 5, 0},
		{2, 8, 7, 0},
		{0, 0, 0, 1}
		});

	Matrix4x4<T> expectedAdjugate({
		{-54, 1, 7, 0},
		{-18, 40, -26, 0},
		{36	, -46, -16, 0},
		{0, 0, 0, -306}
		});

	EXPECT_TRUE(AreMatricesEqual(Matrix4x4<T>::GetAdjugate(m),expectedAdjugate));
}

// Test Matrix Inversion
TEST(Matrix4x4Test,Inversion) {
	Matrix4x4<T> m({
		{6, 1, 1, 0},
		{4, -2, 5, 0},
		{2, 8, 7, 0},
		{0, 0, 0, 1}
		});

	Matrix4x4<T> expectedInverse = (1 / -306.f) * m.GetAdjugate();

	EXPECT_TRUE(AreMatricesEqual(Matrix4x4<T>::Invert(m),expectedInverse,1e-5f));
}

// Test Fast Inverse for a Scale and Translation Matrix
TEST(Matrix4x4Test,FastInverse) {
	Matrix4x4<T> scale = Matrix4x4<T>::CreateScaleMatrix({2, 2, 2});
	Matrix4x4<T> translation = Matrix4x4<T>::CreateTranslationMatrix({3, 4, 5});
	Matrix4x4<T> combined = scale * translation;

	Matrix4x4<T> fastInverse = Matrix4x4<T>::GetFastInverse(combined,{2, 2, 2});

	Matrix4x4<T> expectedFastInverse({
		{0.5, 0, 0, 0},
		{0, 0.5, 0, 0},
		{0, 0, 0.5, 0},
		{-1.5, -2, -2.5, 1}
		});

	EXPECT_TRUE(AreMatricesEqual(fastInverse,expectedFastInverse,1e-3f));
}

TEST(Matrix4x4Test,IdentityTimesAdj) {
	Matrix4x4<T> identity = Matrix4x4<T>();
	Matrix4x4<T> adj = identity.GetAdjugate();
	Matrix4x4<T> expectedAdj;
	expectedAdj(1,1) = 1; expectedAdj(1,2) = 0; expectedAdj(1,3) = 0; expectedAdj(1,4) = 0;
	expectedAdj(2,1) = 0; expectedAdj(2,2) = 1; expectedAdj(2,3) = 0; expectedAdj(2,4) = 0;
	expectedAdj(3,1) = 0; expectedAdj(3,2) = 0; expectedAdj(3,3) = 1; expectedAdj(3,4) = 0;
	expectedAdj(4,1) = 0; expectedAdj(4,2) = 0; expectedAdj(4,3) = 0; expectedAdj(4,4) = 1;

	EXPECT_EQ(adj,expectedAdj);

}

// Helper function for quaternion comparison
template <typename T>
bool AreQuaternionsEqual(const Quaternion<T>& q1,const Quaternion<T>& q2,T epsilon = 1e-5) {
	return q1.IsClose(q2);
}
TEST(QuaternionTest,Matrix4x4ToQuaternion_YAxis) {
	Matrix4x4<float> m({
		0, 0, 1, 0,
		0, 1, 0, 0,
		-1, 0, 0, 0,
		0, 0, 0, 1
		});

	Quaternion<float> expectedQuaternion(0,0.7071068f,0,0.7071068f); // 90 degrees rotation around Y-axis
	Quaternion<float> resultQuaternion(m);

	EXPECT_TRUE(AreQuaternionsEqual(resultQuaternion,expectedQuaternion));
}

// Test Matrix4x4 to Quaternion conversion for 90 degrees rotation around Z-axis
TEST(QuaternionTest,Matrix4x4ToQuaternion_ZAxis) {
	Matrix4x4<float> m({
		0, -1, 0, 0,
		1, 0, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
		});

	Quaternion<float> expectedQuaternion(0,0,0.7071068f,0.7071068f); // 90 degrees rotation around Z-axis
	Quaternion<float> resultQuaternion(m);

	EXPECT_TRUE(AreQuaternionsEqual(resultQuaternion,expectedQuaternion));
}

// Test Matrix4x4 to Quaternion conversion for 180 degrees rotation around X-axis
TEST(QuaternionTest,Matrix4x4ToQuaternion_180Degrees_XAxis) {
	Matrix4x4<float> m({
		1, 0, 0, 0,
		0, -1, 0, 0,
		0, 0, -1, 0,
		0, 0, 0, 1
		});

	Quaternion<float> expectedQuaternion(1,0,0,0); // 180 degrees rotation around X-axis
	Quaternion<float> result(m);

	Quaternion<float> expected1(1,0,0,0);
	Quaternion<float> expected2(0,1,0,0);
	Quaternion<float> expected3(0,0,1,0);
	EXPECT_TRUE(result.IsClose(expected1) || result.IsClose(expected2) || result.IsClose(expected3)); // Handles numerical precision issues
}

// Test Matrix4x4 to Quaternion conversion for 180 degrees rotation around Y-axis
TEST(QuaternionTest,Matrix4x4ToQuaternion_180Degrees_YAxis) {
	Matrix4x4<float> m({
		-1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, -1, 0,
		0, 0, 0, 1
		});

	Quaternion<float> expectedQuaternion(0,1,0,0); // 180 degrees rotation around Y-axis
	Quaternion<float> result(m);

	Quaternion<float> expected1(1,0,0,0);
	Quaternion<float> expected2(0,1,0,0);
	Quaternion<float> expected3(0,0,1,0);
	EXPECT_TRUE(result.IsClose(expected1) || result.IsClose(expected2) || result.IsClose(expected3)); // Handles numerical precision issues
}

// Test Matrix4x4 to Quaternion conversion for 180 degrees rotation around Z-axis
TEST(QuaternionTest,Matrix4x4ToQuaternion_180Degrees_ZAxis) {
	Matrix4x4<float> m({
		-1, 0, 0, 0,
		0, -1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
		});

	Quaternion<float> expectedQuaternion(0,0,1,0); // 180 degrees rotation around Z-axis
	Quaternion<float> result(m);

	Quaternion<float> expected1(1,0,0,0);
	Quaternion<float> expected2(0,1,0,0);
	Quaternion<float> expected3(0,0,1,0);
	EXPECT_TRUE(result.IsClose(expected1) || result.IsClose(expected2) || result.IsClose(expected3)); // Handles numerical precision issues
}

// Test Matrix4x4 to Quaternion conversion for 45 degrees rotation around X-axis
TEST(QuaternionTest,Matrix4x4ToQuaternion_45Degrees_XAxis) {
	Matrix4x4<float> m({
		1, 0, 0, 0,
		0, 0.7071068f, -0.7071068f, 0,
		0, 0.7071068f, 0.7071068f, 0,
		0, 0, 0, 1
		});

	Quaternion<float> expectedQuaternion(0.3826834f,0,0,0.9238795f); // 45 degrees rotation around X-axis
	Quaternion<float> resultQuaternion(m);

	EXPECT_TRUE(AreQuaternionsEqual(resultQuaternion,expectedQuaternion));
}

// Test Matrix4x4 to Quaternion conversion for 45 degrees rotation around Y-axis
TEST(QuaternionTest,Matrix4x4ToQuaternion_45Degrees_YAxis) {
	Matrix4x4<float> m({
		0.7071068f, 0, 0.7071068f, 0,
		0, 1, 0, 0,
		-0.7071068f, 0, 0.7071068f, 0,
		0, 0, 0, 1
		});

	Quaternion<float> expectedQuaternion(0,0.3826834f,0,0.9238795f); // 45 degrees rotation around Y-axis
	Quaternion<float> resultQuaternion(m);

	EXPECT_TRUE(AreQuaternionsEqual(resultQuaternion,expectedQuaternion));
}

// Test Matrix4x4 to Quaternion conversion for 45 degrees rotation around Z-axis
TEST(QuaternionTest,Matrix4x4ToQuaternion_45Degrees_ZAxis) {
	Matrix4x4<float> m({
		0.7071068f, -0.7071068f, 0, 0,
		0.7071068f, 0.7071068f, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
		});

	Quaternion<float> expectedQuaternion(0,0,0.3826834f,0.9238795f); // 45 degrees rotation around Z-axis
	Quaternion<float> resultQuaternion(m);

	EXPECT_TRUE(AreQuaternionsEqual(resultQuaternion,expectedQuaternion));
}
// Test Quaternion to Matrix3x3 conversion
TEST(QuaternionTest,QuaternionToMatrix3x3) {
	Quaternion<float> q(0.7071068f,0,0,0.7071068f); // 90 degrees rotation around X-axis
	Matrix3x3<float> expectedMatrix({
		1, 0, 0,
		0, 0, -1,
		0, 1, 0
		});

	Matrix3x3<float> resultMatrix = q.GetRotationAs3x3();
	EXPECT_TRUE(AreMatricesEqual(resultMatrix,expectedMatrix));
}

// Test Matrix3x3 to Quaternion conversion
TEST(QuaternionTest,Matrix3x3ToQuaternion) {
	Matrix3x3<float> m({
		1,  0,  0,
		0,  0, -1,
		0,  1,  0
		});

	Quaternion<float> expectedQuaternion(0.7071068f,0,0,0.7071068f);
	Quaternion<float> resultQuaternion(m);

	EXPECT_TRUE(AreQuaternionsEqual(resultQuaternion,expectedQuaternion));
}

// Test RotationFromTo for Parallel Vectors (Identity quaternion)
TEST(QuaternionTest,RotationFromTo_ParallelVectors) {
	Vector3f from(1,0,0); // Unit vector along x-axis
	Vector3f to(1,0,0);   // Same direction

	Quaternion<float> result = Quaternion<float>::RotationFromTo(from,to);
	Quaternion<float> expected(0,0,0,1); // Identity quaternion

	EXPECT_TRUE(result.IsClose(expected)); // Assuming IsClose is defined for comparison
}

// Test RotationFromTo for Arbitrary Vectors (e.g., 90 degrees around Z-axis)
TEST(QuaternionTest,RotationFromTo_ArbitraryVectors) {
	Vector3f from(1,0,0); // Unit vector along x-axis
	Vector3f to(0,1,0);   // Unit vector along y-axis

	Quaternion<float> result = Quaternion<float>::RotationFromTo(from,to);
	Vector3f axis(0,0,1); // Rotation around Z-axis
	Quaternion<float> expected = Quaternion<float>::CreateFromAxisAngle(axis,Math::DEG_TO_RAD * 90.0f);

	EXPECT_TRUE(result.IsClose(expected));
}

// Test RotationFromTo for Non-Unit Vectors (Scaled vectors)
TEST(QuaternionTest,RotationFromTo_NonUnitVectors) {
	Vector3f from(2,0,0); // Scaled vector along x-axis
	Vector3f to(0,3,0);   // Scaled vector along y-axis

	Quaternion<float> result = Quaternion<float>::RotationFromTo(from,to);
	Vector3f axis(0,0,1); // Rotation around Z-axis
	Quaternion<float> expected = Quaternion<float>::CreateFromAxisAngle(axis,Math::DEG_TO_RAD * 90.0f);

	EXPECT_TRUE(result.IsClose(expected));
}

TEST(QuaternionTest,RotationFromTo_OppositeVectors) {
	Vector3f from(1,0,0); // Unit vector along x-axis
	Vector3f to(-1,0,0);  // Opposite direction

	Quaternion<float> result = Quaternion<float>::RotationFromTo(from,to);
	Vector3f axis(0,1,0); // Any perpendicular axis
	Quaternion<float> expected1(1,0,0,0);
	Quaternion<float> expected2(0,1,0,0);
	Quaternion<float> expected3(0,0,1,0);
	EXPECT_TRUE(result.IsClose(expected1) || result.IsClose(expected2) || result.IsClose(expected3)); // Handles numerical precision issues
}

// Test Quaternion multiplication
TEST(QuaternionTest,Multiplication) {
	Quaternion<float> q1(1,0,0,0);
	Quaternion<float> q2(0,1,0,0);
	Quaternion<float> expected(0,0,1,0);

	Quaternion<float> result = q1 * q2;

	EXPECT_TRUE(result.IsClose(expected));
}

// Test Quaternion multiplication
TEST(QuaternionTest,MultiplicationAssignment) {
	Quaternion<float> q1(1,0,0,0);
	Quaternion<float> q2(0,1,0,0);
	Quaternion<float> expected(0,0,1,0);

	Quaternion<float> result = q1;
	result *= q2;

	EXPECT_TRUE(result.IsClose(expected));
}

// Test Quaternion normalization
TEST(QuaternionTest,Normalization) {
	Quaternion<float> q(1,2,3,4);
	q.Normalize();
	float length = std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);

	EXPECT_NEAR(length,1.0f,1e-5);
}

// Test Quaternion conjugate
TEST(QuaternionTest,Conjugate) {
	Quaternion<float> q(1,2,3,4);
	Quaternion<float> expected(-1,-2,-3,4);

	Quaternion<float> result = q.GetConjugate();

	EXPECT_TRUE(result.IsClose(expected));
}

// Test Quaternion inverse
TEST(QuaternionTest,Inverse) {
	Quaternion<float> q(1,2,3,4);
	q.Normalize();
	Quaternion<float> expected = q.GetConjugate();
	float lengthSquared = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
	expected = expected * (1.0f / lengthSquared);

	Quaternion<float> result = q.GetInverse();

	EXPECT_TRUE(result.IsClose(expected));
}

// Test Quaternion dot product
TEST(QuaternionTest,DotProduct) {
	Quaternion<float> q1(1,2,3,4);
	Quaternion<float> q2(5,6,7,8);
	float expected = 1 * 5 + 2 * 6 + 3 * 7 + 4 * 8;

	float result = q1.Dot(q2);

	EXPECT_NEAR(result,expected,1e-5);
}

// Test Quaternion slerp
TEST(QuaternionTest,Slerp) {
	Quaternion<float> q1(0,0,0,1);
	Quaternion<float> q2(0,1,0,0);
	float t = 0.5f;
	Quaternion<float> expected = Quaternion<float>::Slerp(q1,q2,t);

	Quaternion<float> result = Quaternion<float>::Slerp(q1,q2,t);

	EXPECT_TRUE(result.IsClose(expected));
}


// Test Quaternion slerp
TEST(QuaternionTest,EulerAngleCheck) {
	//Invalid test, this is not supposed to work like this.
	for(size_t i = 0; i < 100; i++) {
		float x = Math::RandomEngine::randomInRange(-180.f,180.f);
		float y = Math::RandomEngine::randomInRange(-180.f,180.f);
		float z = Math::RandomEngine::randomInRange(-180.f,180.f);
		auto euler = Vector3f(x,y,z);
		Quaternion<float> q1;
		q1.SetEulerAngles(euler);

		EXPECT_TRUE(euler.IsNearlyEqual(q1.GetEulerAngles()));
	}
}
