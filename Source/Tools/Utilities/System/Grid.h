#pragma once
#include <vector>   

#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>

class Grid;
class GridCell;
class GridObject;
class Border
{
public:
	bool ContainsPoint(Vector2<float> aPosition) const;
	Vector2f myPosition;
	Vector2f mySize;
};
class GridObject
{
public:
	Border myBorder;
	Vector4f myColor;
	std::vector<GridCell*> myCells;
};

class GridCell
{
public:
	GridCell() = delete;
	GridCell(const Vector2<float> position, const float size);

	Vector2<float> myPosition;
	float myHalfWidth;
	GridCell* myChildren[4];

	std::vector<GridObject*> myObjects;
	//std::vector<Tga::LinePrimitive> myQuad;
	 
	bool Contains(const Border& aObject);
	bool ContainsPart(const Border& aObject);

	bool AddObject(GridObject* aObject);
	void Subdivide();
	//void Render(Tga::LineDrawer& aLineDrawer, Tga::SpriteSharedData& sbs, Tga::Sprite2DInstanceData& aInstance, Vector2f aCenterOffset);
	std::vector<GridObject*> GetAllWithinRadius(const Border& aBoarder);
	std::vector<GridCell*>  GetCellWithinRadius(const Border& aBoarder);
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
	void Update();
	void Render(); 

	Vector2f GetOffset() const;
	void AddObject(GridObject* aObject);

	std::vector<GridObject*> GetAllWithinRadius(const Border& aBoarder) const;
	std::vector<GridCell*>  GetCellWithinRadius(const Border& aBoarder) const;

	static std::vector<GridObject*> GetObjectsInCells(const std::vector<GridCell*>& aCells);
private:
	const float myGridSize;
	Vector2f myCenterOffset;
	GridCell* myRoot;
	int count; 
}; 