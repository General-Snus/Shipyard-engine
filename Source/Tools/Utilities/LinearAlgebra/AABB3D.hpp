#pragma once 
#include "Sphere.hpp"
#include "Vectors.hpp" 

template <class T = float>
class AABB3D {
public:
	// Default constructor: there is no AABB, both min and max points are the zero vector.
	AABB3D();
	// Copy constructor.
	AABB3D(const AABB3D<T>& aAABB3D); 
	AABB3D(const Vector3<T>& center,const Vector3<T>& extent);
	// Init the AABB with the positions of the minimum and maximum corners, same as
	// the constructor above.
	void InitWithMinAndMax(const Vector3<T>& aMin,const Vector3<T>& aMax);
	// Returns whether a point is inside the AABB: it is inside when the point is on any
	// of the AABB's sides or inside of the AABB.
	bool IsInside(const Vector3<T>& aPosition) const;

	float      DistanceTo(const Vector3<T>& aPosition) const;
	Vector3<T> ClosestPoint(const Vector3<T>& aPosition) const;

	Sphere<T> GetBoundingSphere() const { return Sphere<T>(GetCenter(),GetRadius()); }

	void       Extend(AABB3D<T> aAABB3D);

	Vector3<T>& GetCenter() { return center; }
	Vector3<T>& GetExtent() { return extent; } 
	const  Vector3<T>& GetCenter() const { return center; }
	const  Vector3<T>& GetExtent() const { return extent; }

	T          GetRadius() const { return extent.Length(); }
	T          GetXSize() const { return (extent.x * 2); }
	T          GetYSize() const { return (extent.y * 2); }
	T          GetZSize() const { return (extent.z * 2); }

	Vector3<T> GetMin() const;
	Vector3<T> GetMax() const;
private:
	Vector3<T> center;
	Vector3<T> extent;
};

template <class T>
AABB3D<T>::AABB3D() {
	center = Vector3<T>();
	extent = Vector3<T>(.5,.5,.5);
}

template <class T>
AABB3D<T>::AABB3D(const Vector3<T>& aCenter,const Vector3<T>& aExtent) : center(aCenter),extent(aExtent) {
}

template <class T>
AABB3D<T>::AABB3D(const AABB3D<T>& aAABB3D) {
	center = aAABB3D.center;
	extent = aAABB3D.extent;
}
 
template <class T>
void AABB3D<T>::InitWithMinAndMax(const Vector3<T>& aMin,const Vector3<T>& aMax) {
	const auto MinPoint = MinVector(aMin,aMax);
	const auto MaxPoint = MaxVector(aMin,aMax);
	const auto avg = (MinPoint + MaxPoint) * T(.5);

	center = avg;
	extent = MaxPoint - center;
}

template <class T>
bool AABB3D<T>::IsInside(const Vector3<T>& aPosition) const {
	const auto MinPoint = GetMin();
	const auto MaxPoint = GetMax();

	if(	aPosition.x >= MinPoint.x && aPosition.x <= MaxPoint.x &&
		aPosition.y >= MinPoint.y && aPosition.y <= MaxPoint.y &&
		aPosition.z >= MinPoint.z && aPosition.z <= MaxPoint.z) {
		return true;
	} 

	return false;
}

template <class T>
AABB3D<T> operator* (const AABB3D<T>& subject,T object) {
	return AABB3D<T>(
		subject.GetMin() * subject,
		subject.GetMax() * subject
	);
}

template <class T>
void operator*= (AABB3D<T>& subject,T object) {
	subject = AABB3D<T>(
		subject.GetMin() * object,
		subject.GetMax() * object
	);
}

template <class T>
float AABB3D<T>::DistanceTo(const Vector3<T>& aPosition) const {
	const auto MinPoint = GetMin();
	const auto MaxPoint = GetMax();

	const float dx = std::max<T>(std::max<T>(MinPoint.x - aPosition.x,aPosition.x - MaxPoint.x),0);
	const float dy = std::max<T>(std::max<T>(MinPoint.y - aPosition.y,aPosition.y - MaxPoint.y),0);
	return std::sqrt(dx * dx + dy * dy);
}

template <class T>
Vector3<T> AABB3D<T>::ClosestPoint(const Vector3<T>& aPosition) const {
	const auto MinPoint = GetMin();
	const auto MaxPoint = GetMax();

	Vector3 pointOnBounds = aPosition;
	pointOnBounds.x = std::clamp<T>(aPosition.x,MinPoint.x,MaxPoint.x);
	pointOnBounds.y = std::clamp<T>(aPosition.y,MinPoint.y,MaxPoint.y);
	pointOnBounds.z = std::clamp<T>(aPosition.z,MinPoint.z,MaxPoint.z);
	return pointOnBounds;
}

template <class T>
void AABB3D<T>::Extend(AABB3D<T> aAABB3D) {
	auto MinPoint = GetMin();
	auto MaxPoint = GetMax();

	MinPoint.x = std::min(MinPoint.x,aAABB3D.GetMin().x);
	MinPoint.y = std::min(MinPoint.y,aAABB3D.GetMin().y);
	MinPoint.z = std::min(MinPoint.z,aAABB3D.GetMin().z);

	MaxPoint.x = std::max(MaxPoint.x,aAABB3D.GetMax().x);
	MaxPoint.y = std::max(MaxPoint.y,aAABB3D.GetMax().y);
	MaxPoint.z = std::max(MaxPoint.z,aAABB3D.GetMax().z);

	InitWithMinAndMax(MinPoint,MaxPoint);
}


template <class T>
Vector3<T> AABB3D<T>::GetMin() const { 
	return center-extent;
}

template <class T>
Vector3<T> AABB3D<T>::GetMax() const {
	return center + extent;
}
  