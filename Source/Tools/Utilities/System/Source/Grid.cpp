#include "../Grid.h"
#include <algorithm>
#include <Tools/Utilities/Color.h>
#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>



bool Border2D::ContainsPoint(const Vector2f& aPosition) const
{
	const Vector2f MinPoint = { center.x - size.x, center.y - size.y };
	const Vector2f MaxPoint = { center.x + size.x, center.y + size.y };
	if (aPosition.x >= MinPoint.x && aPosition.x <= MaxPoint.x)
	{
		if (aPosition.y >= MinPoint.y && aPosition.y <= MaxPoint.y)
		{
			return true;
		}
	}
	return false;
}
