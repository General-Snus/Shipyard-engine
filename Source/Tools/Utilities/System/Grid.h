#pragma once
#include <vector>

#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>

class Grid;
class GridCell;
class GridObject;

class Border
{
public:
	bool     ContainsPoint(Vector2<float> aPosition) const;
	Vector2f myPosition;
	Vector2f mySize;
};

class GridObject
{
public:
	Border                 myBorder;
	Vector4f               myColor;
	std::vector<GridCell*> myCells;
};

class GridCell
{
public:
	GridCell() = delete;
	GridCell(Vector2<float> position, float size);

	Vector2<float> myPosition;
	float          myHalfWidth;
	GridCell*      myChildren[4];

	std::vector<GridObject*> myObjects;
	//std::vector<Tga::LinePrimitive> myQuad;

	bool contains(const Border& aObject) const;
	bool containsPart(const Border& aObject) const;

	bool AddObject(GridObject* aObject);
	void Subdivide();
	//void Render(Tga::LineDrawer& aLineDrawer, Tga::SpriteSharedData& sbs, Tga::Sprite2DInstanceData& aInstance, Vector2f aCenterOffset);
	std::vector<GridObject*> getAllWithinRadius(const Border& aBoarder);
	std::vector<GridCell*>   getCellWithinRadius(const Border& aBoarder);
};

class Grid
{
	friend GridCell;

public:
	Grid(
		float aStartX,
		float aStartY,
		float aGridSize
	);
	void update();
	void render();

	Vector2f GetOffset() const;
	void     AddObject(GridObject* aObject);

	std::vector<GridObject*> GetAllWithinRadius(const Border& aBoarder) const;
	std::vector<GridCell*>   GetCellWithinRadius(const Border& aBoarder) const;

	static std::vector<GridObject*> GetObjectsInCells(const std::vector<GridCell*>& aCells);

private:
	const float grid_size;
	Vector2f    center_offset;
	GridCell*   root;
	int         count;
};
