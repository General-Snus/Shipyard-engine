#pragma once
#include "../LinearAlgebra/Frustrum.h"
#include "../LinearAlgebra/Vectors.hpp"
#include "../TemplateHelpers.h"
#include <concepts>
#include <unordered_map>
#include <vector>
#include "../Extensions.h"

struct SpacialHashGridVoxel {
	Vector3i Position;

	bool operator==(const SpacialHashGridVoxel& other) const noexcept
	{
		return Position == other.Position;
	}
};


template <>
struct std::hash<SpacialHashGridVoxel> {
	std::size_t operator()(const SpacialHashGridVoxel& s) const noexcept
	{
		std::size_t seed = 0;
		Extensions::hash_combine(seed, s.Position.x);
		Extensions::hash_combine(seed, s.Position.y);
		Extensions::hash_combine(seed, s.Position.z);
		return seed;
	}
};

template<auto X, auto  Y, auto  Z>
	requires PositiveInt<X>&& PositiveInt<Y>&& PositiveInt<Z>
class SpacialHashGrid
{


	static_assert(PositiveInt<X>&& PositiveInt<Y>&& PositiveInt<Z>, "X, Y, Z must be positive integers");
public:
	static SpacialHashGrid Create(const Vector3f originPosition)
	{
		SpacialHashGrid grid;
		grid.originPosition = originPosition;
		grid.voxelSize = Vector3f(1.0f / static_cast<float>(X), 1.0f / static_cast<float>(Y), 1.0f / static_cast<float>(Z));
		return grid;
	}

	template<typename T>
	void Add(const Vector3f& position, T* object);
	template<typename T>
	void Remove(const Vector3f& position, T* object);
	template<typename T>
	void Update(const Vector3f& position, const Vector3f& newPosition, T* object);

	template<typename T>
	std::vector<T*> QueryAt(const Vector3f& position) const;

	template<typename T>
	std::vector<T*> QueryRadius(const Vector3f& position, float radius) const;

	template<typename T>
	std::vector<T*> QueryFrustrum(const Vector3f& position, Frustum<float> frustrum) const;

	template<typename T>
	void Add(const Vector3f& position, T object);

	template<typename T>
	void Remove(const Vector3f& position, T object);

	template<typename T>
	void Update(const Vector3f& oldPosition, const Vector3f& newPosition, T object);
private:
	SpacialHashGrid();
	using voxelContainer = std::vector<void*>;
	std::unordered_map<SpacialHashGridVoxel, voxelContainer > grid;
	Vector3f originPosition;
	Vector3f voxelSize;
};

template<auto X, auto  Y, auto  Z>
	requires PositiveInt<X >&& PositiveInt<Y>&& PositiveInt<Z>
template<typename T>
inline void SpacialHashGrid<X, Y, Z>::Add(const Vector3f& position, T* object)
{
	SpacialHashGridVoxel voxel;
	auto inverseVoxelSize = Vector3f(1.0f / voxelSize.x, 1.0f / voxelSize.y, 1.0f / voxelSize.z);
	auto inGridPosition = (position - originPosition) * inverseVoxelSize;
	voxel.Position = Vector3i(
		Cast<int>(std::floor(inGridPosition.x)),
		Cast<int>(std::floor(inGridPosition.y)),
		Cast<int>(std::floor(inGridPosition.z))
	);
	grid[voxel].push_back((void*)object);
};
