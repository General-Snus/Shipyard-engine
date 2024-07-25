#pragma once  
#include <Engine/AssetManager/Reflection/ReflectionTemplate.h>
#include "Tools/Utilities/LinearAlgebra/Matrix4x4.h"

class alignas(16) SpotLight : Reflectable
{
private:
	MYLIB_REFLECTABLE();
	int64_t alignment{};
public:
	Vector3f Color = { 1,1,1 };
	float Power = 15;

	Vector3f Position = { 0,0,0 };	//12
	float Range = 30;		//4

	Vector3f Direction = { 0,0,1 }; //12
	float InnerConeAngle = 15; // 4

	float OuterConeAngle = 45; //4  
	int shadowMapIndex = -1;

	alignas(4) bool castShadow = true;;
	int32_t padding8bytes{};

	Matrix lightView{};
	Matrix projection{};
};

REFL_AUTO(
	type(SpotLight),
	field(Color),
	field(Power),
	field(Position),
	field(Range),
	field(Direction),
	field(InnerConeAngle),
	field(OuterConeAngle)
)

struct aligned_int // I love 16 byte alignment
{
	aligned_int() = default;
	aligned_int(int val) : value(val) {}
	void operator= (int val) { value = val; }
	void operator= (aligned_int val) { value = val.value; }
	aligned_int(std::initializer_list<int> v) : value((int)*v.begin()) {}
	int value{};
private:
	const int padding1{0};
	const int padding2{0};
	const int padding3{0};

};

class alignas(16) PointLight : Reflectable
{ 
private:
	MYLIB_REFLECTABLE();	
	int64_t alignment{};
public:
	Vector3f Color = { 1,1,1 };
	float Power = 10;

	Vector3f Position = { 0,0,0 };
	float Range = 20;

	Matrix lightView[6]{};
	Matrix projection{};

	alignas(4) bool castShadow = true; 
private:
	int32_t alignment8{};
	int64_t alignment16{};
public:
	aligned_int shadowMapIndex[6] = {-1,-1 ,-1 ,-1 ,-1 ,-1 };

};

REFL_AUTO(
	type(PointLight),
	field(Color),
	field(Power),
	field(Position),
	field(Range)
)

class alignas(16) DirectionalLight : public Reflectable
{
private:
	MYLIB_REFLECTABLE();
	int64_t alignment{};
public:
	Vector3f Color = { 1,1,1 };
	float Power = 5; // watt 
	Vector4f Direction = { 0,0,1,1 }; 
	Matrix lightView{};
	Matrix projection{}; 

	int shadowMapIndex = -1;
	alignas(4) bool castShadow = true;
private: 
	int64_t alignment8{};
};

REFL_AUTO(
	type(DirectionalLight),
	field(Color),
	field(Power)
) 