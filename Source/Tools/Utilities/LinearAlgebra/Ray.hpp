#include "Vectors.hpp" 
template<class T = float>
class Ray
{
public:
	// Default constructor: there is no ray, the origin and direction are the
	// zero vector.
	Ray();
	// Copy constructor.
	Ray(const Ray<T>& aRay);
	// Constructor that takes two points that define the ray, the direction is
	// aPoint - aOrigin and the origin is aOrigin.
	Ray(const Vector3<T>& aOrigin,const Vector3<T>& aPoint);
	// Init the ray with two points, the same as the constructor above.
	void InitWith2Points(const Vector3<T>& aOrigin,const Vector3<T>& aPoint);
	// Init the ray with an origin and a direction.
	void InitWithOriginAndDirection(const Vector3<T>& aOrigin,const Vector3<T>& aDirection);
	Vector3<T> GetOrigin() const;
	Vector3<T> GetUnitVector() const;


private:
	Vector3<T> Origin;
	Vector3<T> UnitVector;
};

template<class T>
inline Ray<T>::Ray()
{
	Origin = Vector3<T>(0,0,0);
	UnitVector = Vector3<T>(0,0,0);
}
template<class T>
inline Ray<T>::Ray(const Ray<T>& aRay)
{
	Origin = aRay.Origin;
	UnitVector = aRay.UnitVector;
}
template<class T>
inline Ray<T>::Ray(const Vector3<T>& aOrigin,const Vector3<T>& aPoint)
{
	Origin = aOrigin;
	UnitVector = (aPoint - aOrigin).GetNormalized();
}
template<class T>
inline void Ray<T>::InitWith2Points(const Vector3<T>& aOrigin,const Vector3<T>& aPoint)
{
	Origin = aOrigin;
	UnitVector = (aPoint - aOrigin).GetNormalized();
}
template<class T>
inline void Ray<T>::InitWithOriginAndDirection(const Vector3<T>& aOrigin,const Vector3<T>& aDirection)
{
	Origin = aOrigin;
	UnitVector = aDirection;
}
template<class T>
inline Vector3<T> Ray<T>::GetOrigin() const
{
	return  Origin;
}
template<class T>
inline Vector3<T> Ray<T>::GetUnitVector() const
{
	return UnitVector;
}