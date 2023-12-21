#include "Vectors.hpp" 
#include <algorithm>
template<class T>
class AABB3D
{
public:
	// Default constructor: there is no AABB, both min and max points are the zero vector.
	AABB3D();
	// Copy constructor.
	AABB3D(const AABB3D<T>& aAABB3D);
	// Constructor taking the positions of the minimum and maximum corners.
	AABB3D(const Vector3<T>& aMin,const Vector3<T>& aMax);
	// Init the AABB with the positions of the minimum and maximum corners, same as
	// the constructor above.
	void InitWithMinAndMax(const Vector3<T>& aMin,const Vector3<T>& aMax);
	// Returns whether a point is inside the AABB: it is inside when the point is on any
	// of the AABB's sides or inside of the AABB.
	bool IsInside(const Vector3<T>& aPosition) const;
	float DistanceTo(const Vector3<T>& aPosition) const;
	Vector3<T> GetMin() const;
	Vector3<T> GetMax() const;
private:
	Vector3<T> MinPoint;
	Vector3<T> MaxPoint;
};
template<class T>
AABB3D<T>::AABB3D()
{
	MinPoint = Vector3<T>(-1,-1,-1);
	MaxPoint = Vector3<T>(1,1,1);
}

template<class T>
inline AABB3D<T>::AABB3D(const AABB3D<T>& aAABB3D)
{

	MinPoint = aAABB3D.MinPoint;
	MaxPoint = aAABB3D.MaxPoint;
}

template<class T>
inline AABB3D<T>::AABB3D(const Vector3<T>& aMin,const Vector3<T>& aMax)
{
	MinPoint = aMin;
	MaxPoint = aMax;
}

template<class T>
inline void AABB3D<T>::InitWithMinAndMax(const Vector3<T>& aMin,const Vector3<T>& aMax)
{
	MinPoint = aMin;
	MaxPoint = aMax;
}

template<class T>
inline bool AABB3D<T>::IsInside(const Vector3<T>& aPosition) const
{
	if(aPosition.x >= MinPoint.x && aPosition.x <= MaxPoint.x)
	{
		if(aPosition.y >= MinPoint.y && aPosition.y <= MaxPoint.y)
		{
			if(aPosition.z >= MinPoint.z && aPosition.z <= MaxPoint.z)
			{
				return true;
			}
		}
	}
	return false;
}

template<class T>
inline float AABB3D<T>::DistanceTo(const Vector3<T>& aPosition) const
{
	float dx = std::max<T>(std::max<T>(MinPoint.x - aPosition.x,aPosition.x - MaxPoint.x),0);
	float dy = std::max<T>(std::max<T>(MinPoint.y - aPosition.y,aPosition.y - MaxPoint.y),0);
	return std::sqrt(dx * dx + dy * dy);
}

template<class T>
inline Vector3<T> AABB3D<T>::GetMin() const
{
	return MinPoint;
}

template<class T>
inline Vector3<T>  AABB3D<T>::GetMax() const
{
	return MaxPoint;
}