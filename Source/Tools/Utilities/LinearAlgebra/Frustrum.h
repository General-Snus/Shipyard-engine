#pragma once
#include <array>
#include "Plane.hpp"
#include "AABB3D.hpp"
#include "../TemplateHelpers.h"

template<Arithmetic T> 
class Frustum {
public:
	std::array<Plane<T>, 6> planes;
	 
	bool IntersectsAABB(const AABB3D<T>& box) const
	{
		for (const Plane& plane : planes)
		{
			Vector3<T> point {
				 plane.GetNormal().x >= T(0) ? box.max.x : box.min.x,
				 plane.GetNormal().y >= T(0) ? box.max.y : box.min.y,
				 plane.GetNormal().z >= T(0) ? box.max.z : box.min.z
			};

			if (!plane.IsInside(point))
				return false;
		}
		return true;
	}
};
