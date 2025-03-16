#pragma once
#include "Vectors.hpp"

template <class T>
class Sphere
{
public: 
	Sphere(); 
	Sphere(const Sphere<T>& aSphere); 
	Sphere(const Vector3<T>& aCenter, T aRadius); 
	void InitWithCenterAndRadius(const Vector3<T>& aCenter, T aRadius); 
	bool       IsInside(const Vector3<T>& aPosition) const;
	Vector3<T> GetCenter() const;
	T          GetRadius() const;
	bool       ExpandSphere(const Sphere<T>& sphere);

	Vector3<T> ClosestPoint(const Vector3<T>& aPosition) const
	{
		Vector3<T>  relativeVector = aPosition - Center;
		const float length = relativeVector.Length();
		if (length <= Radius)
		{
			return aPosition;
		}
		return Center + relativeVector * (Radius / length);
	}

private:
	Vector3<T> Center;
	T          Radius;
};


template <class T>
Sphere<T>::Sphere()
{
	Center = Vector3<T>(0, 0, 0);
	Radius = 1;
}

template <class T>
Sphere<T>::Sphere(const Sphere<T>& aSphere)
{
	Center = aSphere.Center;
	Radius = aSphere.Radius;
}

template <class T>
Sphere<T>::Sphere(const Vector3<T>& aCenter, T aRadius)
{
	Center = aCenter;
	Radius = aRadius;
}

template <class T>
bool Sphere<T>::ExpandSphere(const Sphere<T>& sphere)
{
	const float sphereDistance = (sphere.Center + Center).Length();
	const float newMaxRadius = sphere.Radius + sphereDistance;
	if (Radius < newMaxRadius)
	{
		Radius = newMaxRadius;
		return true;
	}
	return false;
}

template <class T>
void Sphere<T>::InitWithCenterAndRadius(const Vector3<T>& aCenter, T aRadius)
{
	Center = aCenter;
	Radius = aRadius;
}

template <class T>
bool Sphere<T>::IsInside(const Vector3<T>& aPosition) const
{
	Vector3<T> relativeVector = aPosition - Center;
	if (relativeVector.LengthSqr() <= (Radius * Radius))
	{
		return true;
	}
	return false;
}

template <class T>
Vector3<T> Sphere<T>::GetCenter() const
{
	return Center;
}

template <class T>
T Sphere<T>::GetRadius() const
{
	return Radius;
}
