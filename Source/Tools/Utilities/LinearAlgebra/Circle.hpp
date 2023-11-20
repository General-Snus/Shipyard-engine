#include "Vectors.hpp"
 
	template<class T>
	class Circle
	{
	public:
		// Default constructor: there is no sphere, the radius is zero and the position is
		// the zero vector.
		Circle();
		// Copy constructor.
		Circle(const Circle<T>& aSphere);
		// Constructor that takes the center position and radius of the sphere.
		Circle(const Vector2<T>& aCenter,T aRadius);
		// Init the sphere with a center and a radius, the same as the constructor above.
		void InitWithCenterAndRadius(const Vector2<T>& aCenter,T aRadius);
		// Returns whether a point is inside the sphere: it is inside when the point is on the
		// sphere surface or inside of the sphere.
		bool IsInside(const Vector2<T>& aPosition) const;
		Vector2<T> GetCenter() const;
		T  GetRadius() const;

	private:
		Vector2<T> Center;
		T Radius;
	};


	template<class T>
	inline Circle<T>::Circle()
	{
		Center = Vector3<T>(0,0);
		Radius = 0;
	}
	template<class T>
	inline Circle<T>::Circle(const Circle<T>& aSphere)
	{
		Center = aSphere.Center;
		Radius = aSphere.Radius;
	}
	template<class T>
	inline Circle<T>::Circle(const Vector2<T>& aCenter,T aRadius)
	{
		Center = aCenter;
		Radius = aRadius;
	}
	template<class T>
	inline void Circle<T>::InitWithCenterAndRadius(const Vector2<T>& aCenter,T aRadius)
	{
		Center = aCenter;
		Radius = aRadius;
	}
	template<class T>
	inline bool Circle<T>::IsInside(const Vector2<T>& aPosition) const
	{
		Vector2<T> relativeVector = aPosition - Center;
		if(relativeVector.LengthSqr() <= (Radius * Radius))
		{
			return true;
		}
		return false;
	}
	
	template<class T>
	inline Vector2<T> Circle<T>::GetCenter() const
	{
		return Center;
	}

	template<class T>
	inline T Circle<T>::GetRadius() const
	{
		return Radius;
	} 