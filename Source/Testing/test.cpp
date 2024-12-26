#include "pch.h"

#include "../Tools/Utilities/LinearAlgebra/Easing.h"
#include "../Tools/Utilities/LinearAlgebra/Vector3.hpp"
#include "../Tools/Utilities/LinearAlgebra/Vector4.hpp"
#include "../Tools/Utilities/LinearAlgebra/Matrix4x4.h"
#include "../Tools/Utilities/LinearAlgebra/VectorX.hpp"

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