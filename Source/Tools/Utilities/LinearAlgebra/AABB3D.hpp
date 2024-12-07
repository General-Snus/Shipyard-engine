#pragma once
#define NOMINMAX
#include <algorithm>
#include "Sphere.hpp"
#include "Vectors.hpp"

class Transform;

template <class T = float>
class AABB3D
{
public:
	// Default constructor: there is no AABB, both min and max points are the zero vector.
	AABB3D();
	// Copy constructor.
	AABB3D(const AABB3D<T>& aAABB3D);
	// Constructor taking the positions of the minimum and maximum corners.
	AABB3D(const Vector3<T>& aMin, const Vector3<T>& aMax);
	// Init the AABB with the positions of the minimum and maximum corners, same as
	// the constructor above.
	void InitWithMinAndMax(const Vector3<T>& aMin, const Vector3<T>& aMax);
	// Returns whether a point is inside the AABB: it is inside when the point is on any
	// of the AABB's sides or inside of the AABB.
	bool IsInside(const Vector3<T>& aPosition) const;

	float      DistanceTo(const Vector3<T>& aPosition) const;
	Vector3<T> ClosestPoint(const Vector3<T>& aPosition) const;

	Sphere<T> GetBoundingSphere() const { return Sphere<T>(GetCenter(), GetRadius()); }

	void       Extend(AABB3D<T> aAABB3D);
	Vector3<T> GetCenter() const { return (MinPoint + MaxPoint) * 0.5f; }
	Vector3<T> GetWidth() const { return (MaxPoint - MinPoint) * 0.5f; }
	T          GetRadius() const { return (MaxPoint - MinPoint).Length() / T(2); }
	T          GetXSize() const { return (MaxPoint.x - MinPoint.x) * 0.5f; }
	T          GetYSize() const { return (MaxPoint.y - MinPoint.y) * 0.5f; }
	T          GetZSize() const { return (MaxPoint.z - MinPoint.z) * 0.5f; }

	Vector3<T> GetMin() const;
	Vector3<T> GetMax() const;

private:
	Vector3<T> MinPoint;
	Vector3<T> MaxPoint;
};

template <class T>
AABB3D<T>::AABB3D()
{
	MinPoint = Vector3<T>(-1, -1, -1);
	MaxPoint = Vector3<T>(1, 1, 1);
}

template <class T>
AABB3D<T>::AABB3D(const AABB3D<T>& aAABB3D)
{
	MinPoint = aAABB3D.MinPoint;
	MaxPoint = aAABB3D.MaxPoint;
}

template <class T>
AABB3D<T>::AABB3D(const Vector3<T>& aMin, const Vector3<T>& aMax)
{
	MinPoint = aMin;
	MaxPoint = aMax;
}

template <class T>
void AABB3D<T>::InitWithMinAndMax(const Vector3<T>& aMin, const Vector3<T>& aMax)
{
	MinPoint = aMin;
	MaxPoint = aMax;
}

template <class T>
bool AABB3D<T>::IsInside(const Vector3<T>& aPosition) const
{
	if (aPosition.x >= MinPoint.x && aPosition.x <= MaxPoint.x)
	{
		if (aPosition.y >= MinPoint.y && aPosition.y <= MaxPoint.y)
		{
			if (aPosition.z >= MinPoint.z && aPosition.z <= MaxPoint.z)
			{
				return true;
			}
		}
	}
	return false;
}

template <class T>
float AABB3D<T>::DistanceTo(const Vector3<T>& aPosition) const
{
	const float dx = std::max<T>(std::max<T>(MinPoint.x - aPosition.x, aPosition.x - MaxPoint.x), 0);
	const float dy = std::max<T>(std::max<T>(MinPoint.y - aPosition.y, aPosition.y - MaxPoint.y), 0);
	return std::sqrt(dx * dx + dy * dy);
}

template <class T>
Vector3<T> AABB3D<T>::ClosestPoint(const Vector3<T>& aPosition) const
{
	Vector3 pointOnBounds = aPosition;
	pointOnBounds.x = std::clamp<T>(aPosition.x, MinPoint.x, MaxPoint.x);
	pointOnBounds.y = std::clamp<T>(aPosition.y, MinPoint.y, MaxPoint.y);
	pointOnBounds.z = std::clamp<T>(aPosition.z, MinPoint.z, MaxPoint.z);
	return pointOnBounds;
}

template <class T>
void AABB3D<T>::Extend(AABB3D<T> aAABB3D)
{
	MinPoint.x = std::min(MinPoint.x, aAABB3D.MinPoint.x);
	MinPoint.y = std::min(MinPoint.y, aAABB3D.MinPoint.y);
	MinPoint.z = std::min(MinPoint.z, aAABB3D.MinPoint.z);

	MaxPoint.x = std::max(MaxPoint.x, aAABB3D.MaxPoint.x);
	MaxPoint.y = std::max(MaxPoint.y, aAABB3D.MaxPoint.y);
	MaxPoint.z = std::max(MaxPoint.z, aAABB3D.MaxPoint.z);
}


template <class T>
Vector3<T> AABB3D<T>::GetMin() const
{
	return MinPoint;
}

template <class T>
Vector3<T> AABB3D<T>::GetMax() const
{
	return MaxPoint;
}
