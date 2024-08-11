#pragma once	
#include "Vectors.hpp"

template<class T>
class Sphere
{
public:
	// Default constructor: there is no sphere, the radius is zero and the position is
	// the zero vector.
	Sphere();
	// Copy constructor.
	Sphere(const Sphere<T>& aSphere);
	// Constructor that takes the center position and radius of the sphere.
	Sphere(const Vector3<T>& aCenter,T aRadius);
	// Init the sphere with a center and a radius, the same as the constructor above.
	void InitWithCenterAndRadius(const Vector3<T>& aCenter,T aRadius);
	// Returns whether a point is inside the sphere: it is inside when the point is on the
	// sphere surface or inside of the sphere.
	bool IsInside(const Vector3<T>& aPosition) const;
	Vector3<T> GetCenter() const;
	T  GetRadius() const;
	bool ExpandSphere(const Sphere<T>& sphere);

	Vector3<T> ClosestPoint(const Vector3<T>& aPosition) const
	{
		Vector3<T> relativeVector = aPosition - Center;
		const float length = relativeVector.Length();
		if(length <= Radius)
		{
			return aPosition;
		}
		return Center + relativeVector * (Radius / length);
	}

private:
	Vector3<T> Center;
	T Radius;
};


template<class T>
inline Sphere<T>::Sphere()
{
	Center = Vector3<T>(0,0,0);
	Radius = 0;
}
template<class T>
inline Sphere<T>::Sphere(const Sphere<T>& aSphere)
{
	Center = aSphere.Center;
	Radius = aSphere.Radius;
}
template<class T>
inline Sphere<T>::Sphere(const Vector3<T>& aCenter,T aRadius)
{
	Center = aCenter;
	Radius = aRadius;
}
template<class T>
inline bool Sphere<T>::ExpandSphere(const Sphere<T>& sphere)
{
	const float sphereDistance = (sphere.Center + Center).Length();
	const float newMaxRadius = sphere.Radius + sphereDistance;
	if(Radius < newMaxRadius)
	{
		Radius = newMaxRadius;
		return true;
	}
	return false;
}

template<class T>
inline void Sphere<T>::InitWithCenterAndRadius(const Vector3<T>& aCenter,T aRadius)
{
	Center = aCenter;
	Radius = aRadius;
}
template<class T>
inline bool Sphere<T>::IsInside(const Vector3<T>& aPosition) const
{
	Vector3<T> relativeVector = aPosition - Center;
	if(relativeVector.LengthSqr() <= (Radius * Radius))
	{
		return true;
	}
	return false;
}

template<class T>
inline Vector3<T> Sphere<T>::GetCenter() const
{
	return Center;
}

template<class T>
inline T Sphere<T>::GetRadius() const
{
	return Radius;
}