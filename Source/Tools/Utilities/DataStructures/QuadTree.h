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
template <typename TreeData>
class QuadTreeCell;

template <typename TreeData>
class TreeObject
{
public:
	TreeObject(TreeData* data);
	Border2D                 border;
	Vector4f                 color;
	std::vector<QuadTreeCell<TreeData>*> cells;
	TreeData* data;
};
template <typename TreeData>
class QuadTreeCell
{
public:
	QuadTreeCell() = delete;
	QuadTreeCell<TreeData>(const Vector2f originPosition, float size);

	Vector2<float> myPosition;
	float          halfWidth;
	std::array<QuadTreeCell<TreeData>*, 4> cells;

	std::vector<TreeObject<TreeData>*> myObjects;

	bool contains(const Border2D& aObject) const;
	bool containsPart(const Border2D& aObject) const;

	bool AddObject(TreeObject<TreeData>* aObject);
	void Subdivide();
	void Draw(DebugDrawer& draw, Vector2<float> aCenterOffset) const;

	std::vector<TreeObject<TreeData>*> getAllWithinRadius(const Border2D& aBoarder) const;

	std::vector<QuadTreeCell*> getCellWithinRadius(const Border2D& aBoarder);
	using PrimitiveHandleOpaque = size_t;
	std::vector<PrimitiveHandleOpaque> debugHandles;
};


template <typename TreeData = void>
class QuadTree
{
	friend QuadTreeCell<TreeData>;

public:
	QuadTree(
		float aStartX,
		float aStartY,
		float aGridSize
	);
	QuadTree();

	void Init(
		float aStartX,
		float aStartY,
		float aGridSize);

	void update();
	void Draw(DebugDrawer& draw) const;

	Vector2<float> GetOffset() const;
	void     AddObject(TreeObject<TreeData>* aObject);

	std::vector<TreeObject<TreeData>*> GetAllWithinRadius(const Border2D& aBoarder) const;
	std::vector<QuadTreeCell<TreeData>*>   GetCellWithinRadius(const Border2D& aBoarder) const;

	static std::vector<TreeObject<TreeData>*> GetObjectsInCells(const std::vector<QuadTreeCell<TreeData>*>& aCells);


private:
	float grid_size;
	Vector2<float>    center_offset;
	QuadTreeCell<TreeData>* root;
	int         count;
};

template<typename TreeData>
inline TreeObject<TreeData>::TreeObject(TreeData* data) : data(data)
{
}

template <typename TreeData>
inline QuadTree<TreeData>::QuadTree(
	float aStartX,
	float aStartY,
	float aGridSize) : grid_size(aGridSize), root(new QuadTreeCell<TreeData>(Vector2f(aStartX, aStartY), aGridSize)), count(0)
{
}

template<typename TreeData>
inline QuadTree<TreeData>::QuadTree()
{
	Init(0.0f, 0.0f, 1000.0f);
}

template<typename TreeData>
inline void QuadTree<TreeData>::Init(float aStartX, float aStartY, float aGridSize)
{
	grid_size = (aGridSize);
	root = (new QuadTreeCell<TreeData>(Vector2f(aStartX, aStartY), aGridSize));
	count = (0);
}

template <typename TreeData>
inline void QuadTree<TreeData>::update()
{
}

template <typename TreeData>
inline void QuadTree<TreeData>::Draw(DebugDrawer& draw) const
{
	root->Draw(draw, center_offset);
}
template <typename TreeData>
inline void QuadTreeCell<TreeData>::Draw(DebugDrawer& draw, Vector2<float> aCenterOffset) const
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
template <typename TreeData>
inline Vector2<float> QuadTree<TreeData>::GetOffset() const
{
	return center_offset;
}

template <typename TreeData>
inline void QuadTree<TreeData>::AddObject(TreeObject<TreeData>* aObject)
{
	if (root->AddObject(aObject))
	{
		count++;
	}
}

template <typename TreeData>
inline std::vector<TreeObject<TreeData>*> QuadTree<TreeData>::GetAllWithinRadius(const Border2D& aBoarder)  const
{
	if (!root->containsPart(aBoarder))
	{
		return {};
	}
	std::vector<TreeObject<TreeData>*> objects;
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

template <typename TreeData>
inline std::vector<QuadTreeCell<TreeData>*> QuadTree<TreeData>::GetCellWithinRadius(const Border2D& aBoarder) const
{
	if (!root->containsPart(aBoarder))
	{
		return {};
	}

	std::vector<QuadTreeCell<TreeData>*> cells;
	cells.push_back(root);

	if (root->cells[0] != nullptr)
	{
		for (auto& cell : root->cells)
		{
			std::vector<QuadTreeCell<TreeData>*> newCells = cell->getCellWithinRadius(aBoarder);
			cells.insert(cells.begin(), newCells.begin(), newCells.end());
		}
	}
	return cells;
}

template <typename TreeData>
inline std::vector<TreeObject<TreeData>*>  QuadTree<TreeData>::GetObjectsInCells(const std::vector<QuadTreeCell<TreeData>*>& cells)
{
	std::vector<TreeObject<TreeData>*> vector;
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

template <typename TreeData>
inline std::vector<TreeObject<TreeData>*> QuadTreeCell<TreeData>::getAllWithinRadius(const Border2D& aBoarder) const
{
	if (containsPart(aBoarder))
	{
		return {};
	}
	//int arrPos = (cellX + (cellY * myCellCountX));

	std::vector<TreeObject<TreeData>*> objects;
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
			std::vector<TreeObject<TreeData>*> newCells = i->getAllWithinRadius(aBoarder);
			objects.insert(objects.end(), newCells.begin(), newCells.end());
		}
	}
	return objects;
}

template <typename TreeData>
inline bool QuadTreeCell<TreeData>::AddObject(TreeObject<TreeData>* aObject)
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


template <typename TreeData>
inline QuadTreeCell<TreeData>::QuadTreeCell(const Vector2f originPosition, const float size) : cells{}
{
	myPosition = originPosition;
	halfWidth = size / 2.0f;

	constexpr float modSize = 0.0f;
	Vector2f        start = {
		originPosition.x + modSize,
		originPosition.y + modSize
	};
	Vector2f end = {
		originPosition.x + size - modSize,
		originPosition.y + size - modSize
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

template <typename TreeData>
inline std::vector<QuadTreeCell<TreeData>*>  QuadTreeCell<TreeData>::getCellWithinRadius(const Border2D& aBoarder)
{
	if (!containsPart(aBoarder))
	{
		return {};
	}
	std::vector<QuadTreeCell*> cellInRadius;
	cellInRadius.push_back(this);
	if (cells[0] != nullptr)
	{
		for (const auto& i : cells)
		{
			std::vector<QuadTreeCell*> newCells = i->getCellWithinRadius(aBoarder);
			cellInRadius.insert(cellInRadius.begin(), newCells.begin(), newCells.end());
		}
	}
	return cellInRadius;
}

template <typename TreeData>
inline void QuadTreeCell<TreeData>::Subdivide()
{
	//Subdvide the area equally into 4 new cells
	cells[0] = new QuadTreeCell(Vector2f(myPosition.x - halfWidth/2.f, myPosition.y - halfWidth/2.f), halfWidth);
	cells[1] = new QuadTreeCell(Vector2f(myPosition.x + halfWidth/2.f, myPosition.y - halfWidth/2.f), halfWidth);
	cells[2] = new QuadTreeCell(Vector2f(myPosition.x - halfWidth/2.f, myPosition.y + halfWidth/2.f), halfWidth);
	cells[3] = new QuadTreeCell(Vector2f(myPosition.x + halfWidth/2.f, myPosition.y + halfWidth/2.f), halfWidth);

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
	std::erase_if(myObjects, [](TreeObject<TreeData>* a) { return (a == nullptr); });
	//myObjects.clear();
}

template <typename TreeData>
inline bool  QuadTreeCell<TreeData>::contains(const Border2D& aObject) const
{
	const bool insideX = (aObject.center.x - aObject.size.x * 0.5f >= myPosition.x) && (aObject.center.x +
		aObject.size.x * 0.5f <= myPosition.x + halfWidth * 2.0f);
	const bool insideY = (aObject.center.y - aObject.size.y * 0.5f >= myPosition.y) && (aObject.center.y +
		aObject.size.y * 0.5f <= myPosition.y + halfWidth * 2.0f);

	return (insideX && insideY);
}

template <typename TreeData>
inline bool QuadTreeCell<TreeData>::containsPart(const Border2D& aObject) const
{
	const bool insideX = (aObject.center.x + aObject.size.x * 0.5f >= myPosition.x) && (aObject.center.x -
		aObject.size.x * 0.5f <= myPosition.x + halfWidth * 2.0f);
	const bool insideY = (aObject.center.y + aObject.size.y * 0.5f >= myPosition.y) && (aObject.center.y -
		aObject.size.y * 0.5f <= myPosition.y + halfWidth * 2.0f);

	return (insideX && insideY);
}

