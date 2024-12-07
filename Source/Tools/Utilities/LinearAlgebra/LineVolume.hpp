#pragma once

#include <vector>
#include "Line.hpp"
#include "Vectors.hpp"

template <class T>
class LineVolume
{
public:
	// Default constructor: empty LineVolume.
	LineVolume();
	// Constructor taking a list of Line that makes up the LineVolume.
	LineVolume(const std::vector<Line<T>>& aLineList);
	// Add a Line to the LineVolume.
	void AddLine(const Line<T>& aLine);
	// Returns whether a point is inside the LineVolume: it is inside when the point is
	// on the line or on the side the normal is pointing away from for all the lines in
	// the LineVolume.
	bool IsInside(const Vector2<T>& aPosition) const;

private:
	std::vector<Line<T>> myLineList;
};

template <class T>
LineVolume<T>::LineVolume()
{
}

template <class T>
LineVolume<T>::LineVolume(const std::vector<Line<T>>& aLineList)
{
	myLineList = aLineList;
}

template <class T>
void LineVolume<T>::AddLine(const Line<T>& aLine)
{
	myLineList.push_back(aLine);
}

template <class T>
bool LineVolume<T>::IsInside(const Vector2<T>& aPosition) const
{
	for (Line<T> line : myLineList)
	{
		T scalar = line.GetNormal().Dot(aPosition - line.GetOrigin());

		if (scalar <= 0)
		{
			continue;
		}
		return false;
	}
	return true;
}
