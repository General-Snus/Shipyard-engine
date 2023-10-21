#pragma once
#include <assert.h>
#include <cmath>
#include <cassert> 
#include <iostream>
#include <algorithm>
namespace CommonUtilities
{
	template <class T>
	class Vector4
	{
	public:
		T x;
		T y;
		T z;
		T w;
		//Creates a null-vector
		Vector4<T>();
		//Creates a vector (aX, aY, aZ)
		Vector4<T>(const T& aX,const T& aY,const T& aZ,const T& aW); 
		Vector4<T>(const Vector3<T>& aVector, const T& aW);

		//Copy constructor (compiler generated)
		Vector4<T>(const Vector4<T>& aVector) = default;
		//Assignment operator (compiler generated)
		Vector4<T>& operator=(const Vector4<T>& aVector4) = default;
		Vector4<T>& operator=(const T& aTypeT);
		//Destructor (compiler generated)
		~Vector4<T>() = default;

		//Implicit cast operator to any datatype, return initializer list of x, y, z
		template <class U> operator U() const;

		//Returns the negated vector
		Vector4<T> operator-() const;

		//Returns the array vector
		T& operator[](int value);
		const T& operator[](int value) const;

		//Returns the squared length of the vector
		T LengthSqr() const;

		//Returns the length of the vector
		T Length() const;

		//Returns a normalized copy of this
		Vector4<T> GetNormalized() const;

		//Normalizes the vector
		void Normalize();
		//Returns the dot product of this and aVector
		T Dot(const Vector4<T>& aVector) const;
	};

	template <class T> inline Vector4<T> Lerp(Vector4<T> start_value,Vector4<T> end_value,float t)
	{
		const float val = std::clamp<float>(t,0,1); //assures that the given parameter "t" is between 0 and 1

		return Vector4<T>(
			start_value.x + (end_value.x - start_value.x) * val,
			start_value.y + (end_value.y - start_value.y) * val,
			start_value.z + (end_value.z - start_value.z) * val,
			start_value.w + (end_value.w - start_value.w) * val
		);
	}

	//Returns the vector sum of aVector0 and aVector1
	template <class T> Vector4<T> operator+(const Vector4<T>& aVector0,const Vector4<T>& aVector1)
	{
		return Vector4<T>(
			aVector0.x + aVector1.x,
			aVector0.y + aVector1.y,
			aVector0.z + aVector1.z,
			aVector0.w + aVector1.w
			);
	}

	//Returns the vector difference of aVector0 and aVector1
	template <class T> Vector4<T> operator-(const Vector4<T>& aVector0,const Vector4<T>& aVector1)
	{
		return Vector4<T>(
			aVector0.x - aVector1.x,
			aVector0.y - aVector1.y,
			aVector0.z - aVector1.z,
			aVector0.w - aVector1.w
			);
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T> Vector4<T> operator*(const Vector4<T>& aVector,const T& aScalar)
	{
		return Vector4<T>(
			aVector.x * aScalar,
			aVector.y * aScalar,
			aVector.z * aScalar,
			aVector.w * aScalar
			);
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T> Vector4<T> operator*(const T& aScalar,const Vector4<T>& aVector)
	{
		return Vector4<T>(
			aVector.x * aScalar,
			aVector.y * aScalar,
			aVector.z * aScalar,
			aVector.w * aScalar
			);
	}

	//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1 / aScalar)
	template <class T> Vector4<T> operator/(const Vector4<T>& aVector,const T& aScalar)
	{
		const T mlt = (1 / aScalar);

		return Vector4<T>(
			aVector.x * mlt,
			aVector.y * mlt,
			aVector.z * mlt,
			aVector.w * mlt
			);
	}

	//Equivalent to setting aVector0 to (aVector0 + aVector1)
	template <class T> void operator+=(Vector4<T>& aVector0,const Vector4<T>& aVector1)
	{
		aVector0.x = aVector0.x + aVector1.x;
		aVector0.y = aVector0.y + aVector1.y;
		aVector0.z = aVector0.z + aVector1.z;
		aVector0.w = aVector0.w + aVector1.w;
	}

	//Equivalent to setting aVector0 to (aVector0 - aVector1)
	template <class T> void operator-=(Vector4<T>& aVector0,const Vector4<T>& aVector1)
	{
		aVector0.x = aVector0.x - aVector1.x;
		aVector0.y = aVector0.y - aVector1.y;
		aVector0.z = aVector0.z - aVector1.z;
		aVector0.w = aVector0.w - aVector1.w;
	}

	//Equivalent to setting aVector to (aVector * aScalar)
	template <class T> void operator*=(Vector4<T>& aVector,const T& aScalar)
	{
		aVector.x = aVector.x * aScalar;
		aVector.y = aVector.y * aScalar;
		aVector.z = aVector.z * aScalar;
		aVector.w = aVector.w * aScalar;
	}

	//Equivalent to setting aVector to (aVector / aScalar)
	template <class T> void operator/=(Vector4<T>& aVector,const T& aScalar)
	{
		aVector.x = aVector.x / aScalar;
		aVector.y = aVector.y / aScalar;
		aVector.z = aVector.z / aScalar;
		aVector.w = aVector.w / aScalar;
	}
	template<class T>
	inline Vector4<T>& Vector4<T>::operator=(const T& aTypeT)
	{
		x,y,z,w = aTypeT;
		return *this;
	}

	template<class T>
	inline Vector4<T> Vector4<T>::operator-() const
	{
		return Vector4<T>(-x,-y,-z,-w);
	}

	template<class T>
	inline T Vector4<T>::LengthSqr() const
	{
 		return static_cast<T>(x * x + y * y + z * z + w * w);
	}

	template<class T>
	inline T Vector4<T>::Length() const
	{
		return static_cast<T>(sqrt(LengthSqr()));
	}

	template<class T>
	inline Vector4<T> Vector4<T>::GetNormalized() const
	{
		const T len = 1 / Length();
		return Vector4<T>(
			x * len,
			y * len,
			z * len,
			w * len
			);
	}

	template<class T>
	inline void Vector4<T>::Normalize()
	{
		const T len = 1 / Length();

		x = x * len;
		y = y * len;
		z = z * len;
		w = w * len;
	}

	template<class T>
	inline T Vector4<T>::Dot(const Vector4<T>& aVector) const
	{
		return  x * aVector.x + y * aVector.y + z * aVector.z + w * aVector.w;
	}

	template<class T>
	inline Vector4<T>::Vector4()
	{
		x = 0;
		y = 0;
		z = 0;
		w = 0;
	}

	template<class T>
	inline Vector4<T>::Vector4(const T& aX,const T& aY,const T& aZ,const T& aW)
	{
		x = aX;
		y = aY;
		z = aZ;
		w = aW;
	}

	template<class T>
	inline Vector4<T>::Vector4(const Vector3<T>& aVector,const T& aW)
	{
		x = aVector.x;
		y = aVector.y;
		z = aVector.z;
		w = aW;
	}
	template<class T>
	template<class U>
	inline Vector4<T>::operator U() const
	{
		return  {x,y,z,w};
	}

	template<class T>
	inline T& Vector4<T>::operator[](int value)
	{
		assert(value >= 0 && value <= 4);
		switch(value)
		{
		case 0:
		{
			return x;
		}
		case 1:
		{
			return y;
		}
		case 2:
		{
			return z;
		}
		case 3:
		{
			return w;
		}
		default:
		{
			return x;
		}
		}
	}

	template<class T>
	inline const T& Vector4<T>::operator[](int value) const
	{
		assert(value >= 0 && value <= 4);
		switch(value)
		{
		case 0:
		{
			return x;
		}
		case 1:
		{
			return y;
		}
		case 2:
		{
			return z;
		}
		case 3:
		{
			return w;
		}
		default:
		{
			return x;
		}
		}
	}

}
typedef CommonUtilities::Vector4<float> Vector4f;