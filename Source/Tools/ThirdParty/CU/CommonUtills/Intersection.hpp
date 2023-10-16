#include "Vectors.hpp" 

#include "Plane.hpp"
#include "AABB3D.hpp" 
#include "Ray.hpp" 
#include "Ray2d.hpp" 
#include "Sphere.hpp"
#include "Circle.hpp"
 
namespace CommonUtilities
{
	// If the ray is parallel to the plane, aOutIntersectionPoint remains unchanged. If
	// the ray is in the plane, true is returned, if not, false is returned. If the ray
	// isn't parallel to the plane, the intersection point is stored in
	// aOutIntersectionPoint and true returned.
	template<class T>
	bool IntersectionPlaneRay(const Plane<T>& aPlane,const Ray<T>& aRay,Vector3<T>& aOutIntersectionPoint)
	{
		Vector3<T> RayOrigin = aRay.GetOrigin();
		Vector3<T> RayDirection = aRay.GetUnitVector();
		Vector3<T> NormalOfPlane = aPlane.GetNormal().GetNormalized();
		Vector3<T> OriginOfPlane = aPlane.GetOrigin();

		T b = RayDirection.Dot(NormalOfPlane);
		T a = NormalOfPlane.Dot(OriginOfPlane - RayOrigin);

		if(b != static_cast<T>(0))
		{
			if(a == static_cast<T>(0))
			{
				aOutIntersectionPoint = RayOrigin;
				return true;
			}

			T t = a / b;
			if(t > 0)
			{
				aOutIntersectionPoint = RayDirection * t + RayOrigin;
				return true;
			}
			return false;
		}
		if(a == 0)
		{
			return true;
		}
		return false;
	}

	// If the ray intersects the AABB, true is returned, if not, false is returned.
	// A ray in one of the AABB's sides is counted as intersecting it.
	template<class T>
	bool IntersectionAABBRay(const AABB3D<T>  aAABB,const Ray<T>& aRay)
	{
		{
#define LEFT	0
#define MIDDLE	1
#define RIGHT	2

			Vector3<T>  minPoint = aAABB.GetMin();
			Vector3<T>  maxPoint = aAABB.GetMax();
			Vector3<T> hitPoint = Vector3<T>(0,0,0);

			Vector3<T> rayOrigin = aRay.GetOrigin();
			Vector3<T> rayDirection = aRay.GetUnitVector();


			{
				bool inside = true;
				char quadrant[3]{};
				int whichPlane;
				Vector3<T> tAtIntersectionOfPlane;
				Vector3<T> candidatePlane;

				//i = x,y,z for less code
				//decides if in the area between min and max
				for(int i = 0; i < 3; i++)
				{
					if(rayOrigin[i] < minPoint[i])
					{
						quadrant[i] = LEFT;
						candidatePlane[i] = minPoint[i];
						inside = false;
					}
					else if(rayOrigin[i] > maxPoint[i])
					{
						quadrant[i] = RIGHT;
						candidatePlane[i] = maxPoint[i];
						inside = false;
					}
					else
					{
						quadrant[i] = MIDDLE;
					}
				}

				// all Quadrants are intersected
				if(inside)
				{
					hitPoint = rayOrigin;
					return true;
				}


				for(int i = 0; i < 3; i++)
				{
					//If i am not in the volume that extrudes from the normal of the face1. and my direction toward that origin is not 0 
					if(quadrant[i] != MIDDLE && rayDirection[i] != 0.)
					{
						tAtIntersectionOfPlane[i] = (candidatePlane[i] - rayOrigin[i]) / rayDirection[i];
					}
					else
					{
						tAtIntersectionOfPlane[i] = -1.;
					}
				}

				//The largest t value is the intersected plane. Check what axis it corresponds to.
				whichPlane = 0;
				for(int i = 1; i < 3; i++)
				{
					if(tAtIntersectionOfPlane[whichPlane] < tAtIntersectionOfPlane[i])
					{
						whichPlane = i;
					}
				}

				if(tAtIntersectionOfPlane[whichPlane] < 0)
				{
					return  false;
				}
				for(int i = 0; i < 3; i++)
				{
					if(whichPlane != i)
					{
						//Set the coordinates in the plane of intersection
						hitPoint[i] = rayOrigin[i] + tAtIntersectionOfPlane[whichPlane] * rayDirection[i];
						if(hitPoint[i] < minPoint[i] || hitPoint[i] > maxPoint[i])
						{
							return false;
						}
					}
					else
					{
						hitPoint[i] = candidatePlane[i];
					}
				}
				return true;
			}
		}
	}

	// If the ray intersects the sphere, true is returned, if not, false is returned.
	// A rat intersecting the surface of the sphere is considered as intersecting it.
	template<class T>
	bool IntersectionSphereRay(const Sphere<T>& aSphere,const Ray<T>& aRay)
	{
		Vector3<T> centerToPoint = aSphere.GetCenter() - aRay.GetOrigin();
		T Scalar = centerToPoint.Dot(aRay.GetUnitVector());
		Vector3<T> ClosestPoint = aRay.GetOrigin() + aRay.GetUnitVector() * Scalar;
		Vector3<T> fromCenterToPoint = ClosestPoint - aSphere.GetCenter();

		if(fromCenterToPoint.Length() <= aSphere.GetRadius())
		{
			return true;
		}
		return false;

	}

	template<class T>
	bool IntersectionCircleRay(const Circle<T>& aCircle,const Ray2d<T>& aRay)
	{
		Vector2<T> centerToPoint = aCircle.GetCenter() - aRay.GetOrigin();
		T Scalar = centerToPoint.Dot(aRay.GetUnitVector());
		const Vector2<T> ClosestPoint = aRay.GetOrigin() + aRay.GetUnitVector() * Scalar;
		Vector2<T> fromCenterToPoint = ClosestPoint - aCircle.GetCenter();

		if(fromCenterToPoint.Length() <= aCircle.GetRadius())
		{
			return true;
		}
		return false;
	}

}
