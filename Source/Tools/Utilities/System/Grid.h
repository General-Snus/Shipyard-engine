#pragma once
#include <vector> 
#include <algorithm>
#include <Tools/Utilities/LinearAlgebra/Vectors.hpp> 
#include <Tools/Utilities/Color.h>
#include <Engine/GraphicsEngine/DebugDrawer/DebugDrawer.h>
class DebugDrawer;

class Border2D
{
public:
	bool     ContainsPoint(const Vector2f& aPosition) const;
	Vector2<float> center;
	Vector2<float> size;
};
template <typename GridData>
class GridCell2D;

template <typename GridData>
class GridObject2D
{
public:
	GridObject2D(GridData* data);
	Border2D                 border;
	Vector4f                 color;
	std::vector<GridCell2D<GridData>*> cells;
	GridData* data;
};
template <typename GridData>
class GridCell2D
{
public:
	GridCell2D() = delete;
	GridCell2D<GridData>(const Vector2f position, float size);

	Vector2<float> myPosition;
	float          halfWidth;
	std::array<GridCell2D<GridData>*, 4> cells;

	std::vector<GridObject2D<GridData>*> myObjects;

	bool contains(const Border2D& aObject) const;
	bool containsPart(const Border2D& aObject) const;

	bool AddObject(GridObject2D<GridData>* aObject);
	void Subdivide();
	void Draw(DebugDrawer& draw, Vector2<float> aCenterOffset) const;

	std::vector<GridObject2D<GridData>*> getAllWithinRadius(const Border2D& aBoarder) const;

	std::vector<GridCell2D*> getCellWithinRadius(const Border2D& aBoarder);
	using PrimitiveHandleOpaque = size_t;
	std::vector<PrimitiveHandleOpaque> debugHandles;
};


template <typename GridData = void>
class Grid2D
{
	friend GridCell2D<GridData>;

public:
	Grid2D(
		float aStartX,
		float aStartY,
		float aGridSize
	);
	Grid2D();

	void Init(
		float aStartX,
		float aStartY,
		float aGridSize);

	void update();
	void Draw(DebugDrawer& draw) const;

	Vector2<float> GetOffset() const;
	void     AddObject(GridObject2D<GridData>* aObject);

	std::vector<GridObject2D<GridData>*> GetAllWithinRadius(const Border2D& aBoarder) const;
	std::vector<GridCell2D<GridData>*>   GetCellWithinRadius(const Border2D& aBoarder) const;

	static std::vector<GridObject2D<GridData>*> GetObjectsInCells(const std::vector<GridCell2D<GridData>*>& aCells);


private:
	float grid_size;
	Vector2<float>    center_offset;
	GridCell2D<GridData>* root;
	int         count;
};

template<typename GridData>
inline GridObject2D<GridData>::GridObject2D(GridData* data) : data(data)
{
}

template <typename GridData>
inline Grid2D<GridData>::Grid2D(
	float aStartX,
	float aStartY,
	float aGridSize) : grid_size(aGridSize), root(new GridCell2D<GridData>(Vector2f(aStartX, aStartY), aGridSize)), count(0)
{
}

template<typename GridData>
inline Grid2D<GridData>::Grid2D()
{
	Init(0.0f, 0.0f, 1000.0f);
}

template<typename GridData>
inline void Grid2D<GridData>::Init(float aStartX, float aStartY, float aGridSize)
{
	grid_size = (aGridSize);
	root = (new GridCell2D<GridData>(Vector2f(aStartX, aStartY), aGridSize));
	count = (0);
}

template <typename GridData>
inline void Grid2D<GridData>::update()
{
}

template <typename GridData>
inline void Grid2D<GridData>::Draw(DebugDrawer& draw) const
{
	root->Draw(draw, center_offset);
}
template <typename GridData>
inline void GridCell2D<GridData>::Draw(DebugDrawer& draw, Vector2<float> aCenterOffset) const
{
	if (cells[0] != nullptr)
	{
		for (const auto& cell : cells)
		{
			cell->Draw(draw, cell->myPosition);
		}
	}

	draw.AddDebugQuad(Vector3f(aCenterOffset.x, 0, aCenterOffset.y), Vector3f(halfWidth, 0, halfWidth), Colors::red.GetRGB(), .1f);
}
template <typename GridData>
inline Vector2<float> Grid2D<GridData>::GetOffset() const
{
	return center_offset;
}

template <typename GridData>
inline void Grid2D<GridData>::AddObject(GridObject2D<GridData>* aObject)
{
	if (root->AddObject(aObject))
	{
		count++;
	}
}

template <typename GridData>
inline std::vector<GridObject2D<GridData>*> Grid2D<GridData>::GetAllWithinRadius(const Border2D& aBoarder)  const
{
	if (!root->containsPart(aBoarder))
	{
		return {};
	}
	std::vector<GridObject2D<GridData>*> objects;
	for (auto i : root->myObjects)
	{
		if (aBoarder.ContainsPoint(i->border.center))
		{
			objects.push_back(i);
		}
	}

	auto additionalObjects = root->getAllWithinRadius(aBoarder);
	objects.insert(objects.end(), additionalObjects.begin(), additionalObjects.end());
	return objects;
}

template <typename GridData>
inline std::vector<GridCell2D<GridData>*> Grid2D<GridData>::GetCellWithinRadius(const Border2D& aBoarder) const
{
	if (!root->containsPart(aBoarder))
	{
		return {};
	}

	std::vector<GridCell2D<GridData>*> cells;
	cells.push_back(root);

	if (root->cells[0] != nullptr)
	{
		for (auto& cell : root->cells)
		{
			std::vector<GridCell2D<GridData>*> newCells = cell->getCellWithinRadius(aBoarder);
			cells.insert(cells.begin(), newCells.begin(), newCells.end());
		}
	}
	return cells;
}

template <typename GridData>
inline std::vector<GridObject2D<GridData>*>  Grid2D<GridData>::GetObjectsInCells(const std::vector<GridCell2D<GridData>*>& cells)
{
	std::vector<GridObject2D<GridData>*> vector;
	for (const auto cell : cells)
	{
		for (const auto& obj : cell->myObjects)
		{
			if (std::ranges::find(vector, obj) == vector.end())
			{
				vector.push_back(obj);
			}
		}
	}

	return vector;
}

template <typename GridData>
inline std::vector<GridObject2D<GridData>*> GridCell2D<GridData>::getAllWithinRadius(const Border2D& aBoarder) const
{
	if (containsPart(aBoarder))
	{
		return {};
	}
	//int arrPos = (cellX + (cellY * myCellCountX));

	std::vector<GridObject2D<GridData>*> objects;
	for (auto& i : myObjects)
	{
		if (aBoarder.ContainsPoint(i->border.center))
		{
			objects.push_back(i);
		}
	}

	if (cells[0] != nullptr)
	{
		for (auto& i : cells)
		{
			std::vector<GridObject2D<GridData>*> newCells = i->getAllWithinRadius(aBoarder);
			objects.insert(objects.end(), newCells.begin(), newCells.end());
		}
	}
	return objects;
}

template <typename GridData>
inline bool GridCell2D<GridData>::AddObject(GridObject2D<GridData>* aObject)
{
	if (!contains(aObject->border))
	{
		return false;
	}

	if (myObjects.size() < 4 && cells[0] == nullptr)
	{
		myObjects.push_back(aObject);
		return true;
	}

	if (cells[0] == nullptr)
	{
		Subdivide();
	}

	for (auto& cell : cells)
	{
		if (cell->AddObject(aObject))
		{
			return true;
		}
	}
	//The object is overlapping with child cells but not fully contained within any of them
	assert("ERROR: Object not added to any child cell" && false);
	myObjects.push_back(aObject);
	return true;
}


template <typename GridData>
inline GridCell2D<GridData>::GridCell2D(const Vector2f position, const float size) : cells{}
{
	myPosition = position;
	halfWidth = size / 2.0f;

	constexpr float modSize = 0.0f;
	Vector2f        start = {
		position.x + modSize,
		position.y + modSize
	};
	Vector2f end = {
		position.x + size - modSize,
		position.y + size - modSize
	};
	//myQuad.push_back(Tga::LinePrimitive());
	//myQuad.back().fromPosition = { start.x,start.y,0 };
	//myQuad.back().toPosition = { start.x, end.y,0 };
	//myQuad.back().color = { 1,0,0,1 };
	//
	//myQuad.push_back(Tga::LinePrimitive());
	//myQuad.back().fromPosition = { start.x,start.y,0 };
	//myQuad.back().toPosition = { end.x,start.y,0 };
	//myQuad.back().color = { 1,0,0,1 };
	//
	//myQuad.push_back(Tga::LinePrimitive());
	//myQuad.back().fromPosition = { end.x,start.y,0 };
	//myQuad.back().toPosition = { end.x, end.y,0 };
	//myQuad.back().color = { 1,0,0,1 };
	//
	//myQuad.push_back(Tga::LinePrimitive());
	//myQuad.back().fromPosition = { start.x, end.y,0 };
	//myQuad.back().toPosition = { end.x,end.y  ,0 };
	//myQuad.back().color = { 1,0,0,1 };
}

template <typename GridData>
inline std::vector<GridCell2D<GridData>*>  GridCell2D<GridData>::getCellWithinRadius(const Border2D& aBoarder)
{
	if (!containsPart(aBoarder))
	{
		return {};
	}
	std::vector<GridCell2D*> cellInRadius;
	cellInRadius.push_back(this);
	if (cells[0] != nullptr)
	{
		for (const auto& i : cells)
		{
			std::vector<GridCell2D*> newCells = i->getCellWithinRadius(aBoarder);
			cellInRadius.insert(cellInRadius.begin(), newCells.begin(), newCells.end());
		}
	}
	return cellInRadius;
}

template <typename GridData>
inline void GridCell2D<GridData>::Subdivide()
{
	//Subdvide the area equally into 4 new cells
	cells[0] = new GridCell2D(Vector2f(myPosition.x - halfWidth/2.f, myPosition.y - halfWidth/2.f), halfWidth);
	cells[1] = new GridCell2D(Vector2f(myPosition.x + halfWidth/2.f, myPosition.y - halfWidth/2.f), halfWidth);
	cells[2] = new GridCell2D(Vector2f(myPosition.x - halfWidth/2.f, myPosition.y + halfWidth/2.f), halfWidth);
	cells[3] = new GridCell2D(Vector2f(myPosition.x + halfWidth/2.f, myPosition.y + halfWidth/2.f), halfWidth);

	//Move objects to the new cells
	for (auto& object : myObjects)
	{
		bool succeded = false;
		for (auto& cell : cells)
		{
			if (cell->AddObject(object))
			{
				succeded = true;
				break;
			}
		}
		if (succeded)
		{
			object = nullptr;
		}
	}
	//bool lamda = [](GridCell* a) {return a == nullptr}
	std::erase_if(myObjects, [](GridObject2D<GridData>* a) { return (a == nullptr); });
	//myObjects.clear();
}

template <typename GridData>
inline bool  GridCell2D<GridData>::contains(const Border2D& aObject) const
{
	const bool insideX = (aObject.center.x - aObject.size.x * 0.5f >= myPosition.x) && (aObject.center.x +
		aObject.size.x * 0.5f <= myPosition.x + halfWidth * 2.0f);
	const bool insideY = (aObject.center.y - aObject.size.y * 0.5f >= myPosition.y) && (aObject.center.y +
		aObject.size.y * 0.5f <= myPosition.y + halfWidth * 2.0f);

	return (insideX && insideY);
}

template <typename GridData>
inline bool GridCell2D<GridData>::containsPart(const Border2D& aObject) const
{
	const bool insideX = (aObject.center.x + aObject.size.x * 0.5f >= myPosition.x) && (aObject.center.x -
		aObject.size.x * 0.5f <= myPosition.x + halfWidth * 2.0f);
	const bool insideY = (aObject.center.y + aObject.size.y * 0.5f >= myPosition.y) && (aObject.center.y -
		aObject.size.y * 0.5f <= myPosition.y + halfWidth * 2.0f);

	return (insideX && insideY);
}

