#include "pch.h"

#include "../Tools/Utilities/LinearAlgebra/Vector3.hpp"
#include "../Tools/Utilities/LinearAlgebra/Vector4.hpp"
#include "../Tools/Utilities/LinearAlgebra/VectorX.hpp"

TEST(TestCaseName, TestName)
{
	const auto intVec = VectorX<int, 3>({1, 2, 3});
	auto expandedIntVec = intVec.swizzle<0, 1, 2, 3, -3>();

	auto floatVec = Vector3f(1.5f, 2.5f, 3.5f);
	// Vector4<float> expandedFloatVec = floatVec.swizzle<zero,-x,z,y>().V4();

	// Vector4f vec4 = expandedFloatVec.V4();
	// Vector4<int> vec4I = expandedIntVec.V4();

	EXPECT_EQ(expandedIntVec[0], 0);
	EXPECT_EQ(expandedIntVec[1], 1);
	EXPECT_EQ(expandedIntVec[2], 2);
	EXPECT_EQ(expandedIntVec[3], 3);

	// EXPECT_NEAR(expandedFloatVec.x, 0, 0.2f);
	// EXPECT_NEAR(expandedFloatVec.y, -floatVec.x, 0.2f);
	// EXPECT_NEAR(expandedFloatVec.z, floatVec.z, 0.2f);
	// EXPECT_NEAR(expandedFloatVec.w, floatVec.y, 0.2f);
}
