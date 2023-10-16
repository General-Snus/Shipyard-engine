#include "Vectors.hpp"
namespace CommonUtilities
{
	template<class T>
	class Ray2d
	{
	public:
		// Default constructor: there is no ray, the origin and direction are the
		// zero vector.
		Ray2d();
		// Copy constructor.
		Ray2d(const Ray2d<T>& aRay);
		// Constructor that takes two points that define the ray, the direction is
		// aPoint - aOrigin and the origin is aOrigin.
		Ray2d(const Vector2<T>& aOrigin,const Vector2<T>& aPoint);
		// Init the ray with two points, the same as the constructor above.
		void InitWith2Points(const Vector2<T>& aOrigin,const Vector2<T>& aPoint);
		// Init the ray with an origin and a direction.
		void InitWithOriginAndDirection(const Vector2<T>& aOrigin,const Vector2<T>& aDirection);
		Vector2<T> GetOrigin() const;
		Vector2<T> GetUnitVector() const;


	private: 
		Vector2<T> Origin;
		Vector2<T> UnitVector;
	};

	template<class T>
	inline Ray2d<T>::Ray2d()
	{
		Origin = Vector2<T>(0,0,0);
		UnitVector = Vector2<T>(0,0,0);
	}
	template<class T>
	inline Ray2d<T>::Ray2d(const Ray2d<T>& aRay)
	{
		Origin = aRay.Origin;
		UnitVector = aRay.UnitVector;
	}
	template<class T>
	inline Ray2d<T>::Ray2d(const Vector2<T>& aOrigin,const Vector2<T>& aPoint)
	{
		Origin = aOrigin;
		UnitVector = (aPoint - aOrigin).GetNormalized();
	}
	template<class T>
	inline void Ray2d<T>::InitWith2Points(const Vector2<T>& aOrigin,const Vector2<T>& aPoint)
	{
		Origin = aOrigin;
		UnitVector = (aPoint - aOrigin).GetNormalized();
	}
	template<class T>
	inline void Ray2d<T>::InitWithOriginAndDirection(const Vector2<T>& aOrigin,const Vector2<T>& aDirection)
	{
		Origin = aOrigin;
		UnitVector = aDirection;
	}
	template<class T>
	inline Vector2<T> Ray2d<T>::GetOrigin() const
	{
		return  Origin;
	}
	template<class T>
	inline Vector2<T> Ray2d<T>::GetUnitVector() const
	{
		return UnitVector;
	}
}
