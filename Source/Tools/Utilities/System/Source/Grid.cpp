#include "../Grid.h" 
#include <algorithm>

Grid::Grid(
	float aStartX,
	float aStartY,
	float aGridSize) : myGridSize(aGridSize), myRoot(new GridCell(Vector2f(aStartX, aStartY), aGridSize)), count(0)
{
}

void Grid::Update()
{
}
void Grid::Render()
{
	//myRoot->Render(dbd, sharedData, instance, myCenterOffset); 
}
Vector2f Grid::GetOffset() const
{
	return myCenterOffset;
}
void Grid::AddObject(GridObject* aObject)
{
	if (myRoot->AddObject(aObject))
	{
		count++;
	}
}

std::vector<GridObject*> Grid::GetAllWithinRadius(const Border& aBoarder) const
{
	if (!myRoot->ContainsPart(aBoarder))
	{
		return {};
	}
	std::vector<GridObject*> cells;
	for (auto& i : myRoot->myObjects)
	{
		if (aBoarder.ContainsPoint(i->myBorder.myPosition))
		{
			cells.push_back(i);
		}
	}

	cells = myRoot->GetAllWithinRadius(aBoarder);
	return cells;
}
std::vector<GridCell*> Grid::GetCellWithinRadius(const Border& aBoarder) const
{
	if (!myRoot->ContainsPart(aBoarder))
	{
		return {};
	}

	std::vector<GridCell*> cells;
	cells.push_back(myRoot);

	if (myRoot->myChildren[0] != nullptr)
	{
		for (const auto& i : myRoot->myChildren)
		{

			std::vector<GridCell*> newCells = i->GetCellWithinRadius(aBoarder);
			cells.insert(cells.begin(), newCells.begin(), newCells.end());
		}
	}
	return cells;
}
std::vector<GridObject*> Grid::GetObjectsInCells(const std::vector<GridCell*>& aCells)
{
	std::vector<GridObject*> vector;
	for (const auto cell : aCells)
	{
		for (auto obj : cell->myObjects)
		{
			if (std::ranges::find(vector, obj) == vector.end())

			{
				vector.push_back(obj);
			}
		}
	}

	return	vector;
}

GridCell::GridCell(const Vector2<float> position, const  float  size) : myChildren{}
{
	myPosition = position;
	myHalfWidth = size / 2.0f;


	constexpr float modSize = 0.0f;
	Vector2f start = {
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

std::vector<GridObject*> GridCell::GetAllWithinRadius(const Border& aBoarder)
{

	if (ContainsPart(aBoarder))
	{
		return {};
	}
	//int arrPos = (cellX + (cellY * myCellCountX));

	std::vector<GridObject*> cells;
	for (auto& i : myObjects)
	{
		if (aBoarder.ContainsPoint(i->myBorder.myPosition))
		{
			cells.push_back(i);
		}
	}
	if (myChildren[0] != nullptr)
	{
		for (const auto& i : myChildren)
		{
			std::vector<GridObject*> newCells = i->GetAllWithinRadius(aBoarder);
			cells.insert(cells.begin(), newCells.begin(), newCells.end());
		}
	}
	return cells;
}
std::vector<GridCell*> GridCell::GetCellWithinRadius(const Border& aBoarder)
{
	if (!ContainsPart(aBoarder))
	{
		return {};
	}
	std::vector<GridCell*> cells;
	cells.push_back(this);
	if (myChildren[0] != nullptr)
	{
		for (const auto& i : myChildren)
		{
			std::vector<GridCell*> newCells = i->GetCellWithinRadius(aBoarder);
			cells.insert(cells.begin(), newCells.begin(), newCells.end());
		}
	}
	return cells;
}
bool GridCell::AddObject(GridObject* aObject)
{
	if (!Contains(aObject->myBorder))
	{
		return false;
	}

	if (myObjects.size() < 4 && myChildren[0] == nullptr)
	{
		myObjects.push_back(aObject);
		return true;
	}

	if (myChildren[0] == nullptr)
	{

		Subdivide();
	}

	for (const auto& i : myChildren)
	{
		if (i->AddObject(aObject))
		{
			return true;
		}
	}
	//The object is overlapping with child cells but not fully contained within any of them
	myObjects.push_back(aObject);
	return true;


	//6assert("ERROR: Object not added to any child cell" && false);
}
void GridCell::Subdivide()
{
	//Subdvide the area equally into 4 new cells
	myChildren[0] = new GridCell(myPosition, myHalfWidth);
	myChildren[1] = new GridCell(Vector2<float>(myPosition.x + myHalfWidth, myPosition.y), myHalfWidth);
	myChildren[2] = new GridCell(Vector2<float>(myPosition.x, myPosition.y + myHalfWidth), myHalfWidth);
	myChildren[3] = new GridCell(Vector2<float>(myPosition.x + myHalfWidth, myPosition.y + myHalfWidth), myHalfWidth);



	//Move objects to the new cells
	for (auto& i : myObjects)
	{
		bool succeded = false;
		for (const auto& j : myChildren)
		{
			if (j->AddObject(i))
			{
				succeded = true;
				continue;
			}
		}
		if (succeded)
		{
			i = nullptr;
		}
	}
	//bool lamda = [](GridCell* a) {return a == nullptr}
	myObjects.erase(std::remove_if(myObjects.begin(), myObjects.end(), [](GridObject* a) { return (a == nullptr); }), myObjects.end());
	//myObjects.clear();
}
//void GridCell::Render(Tga::LineDrawer& aLineDrawer, Tga::SpriteSharedData& sbs, Tga::Sprite2DInstanceData& aInstance, Vector2<float> aCenterOffset)
//{
//	for (int i = 0; i < myObjects.size(); i++)
//	{
//		auto& engine = *Tga::Engine::GetInstance();
//		Tga::SpriteDrawer& spriteDrawer(engine.GetGraphicsEngine().GetSpriteDrawer());
//		aInstance.myPosition = { myObjects[i]->myBorder.myPosition.x + aCenterOffset.x,myObjects[i]->myBorder.myPosition.y + aCenterOffset.y };
//		aInstance.mySize = { myObjects[i]->myBorder.mySize.x  ,myObjects[i]->myBorder.mySize.x };
//		aInstance.myColor = myObjects[i]->myColor;
//		spriteDrawer.Draw(sbs, aInstance);
//		myObjects[i]->myColor = { 1,0,0,1 };
//	}
//
//	if (myChildren[0] != nullptr)
//	{
//		for (int i = 0; i < 4; i++)
//		{
//			myChildren[i]->Render(aLineDrawer, sbs, aInstance, aCenterOffset);
//		}
//	}
//
//	aLineDrawer.Draw(myQuad[0]);
//	aLineDrawer.Draw(myQuad[1]);
//	aLineDrawer.Draw(myQuad[2]);
//	aLineDrawer.Draw(myQuad[3]);
//
//
//	SetColor({ 1,0,0,1 });
//
//}

bool GridCell::Contains(const Border& aObject)
{
	const bool insideX = (aObject.myPosition.x - aObject.mySize.x * 0.5f >= myPosition.x) && (aObject.myPosition.x + aObject.mySize.x * 0.5f <= myPosition.x + myHalfWidth * 2.0f);
	const bool insideY = (aObject.myPosition.y - aObject.mySize.y * 0.5f >= myPosition.y) && (aObject.myPosition.y + aObject.mySize.y * 0.5f <= myPosition.y + myHalfWidth * 2.0f);

	return (insideX && insideY);
}

bool GridCell::ContainsPart(const Border& aObject)
{
	const bool insideX = (aObject.myPosition.x + aObject.mySize.x * 0.5f >= myPosition.x) && (aObject.myPosition.x - aObject.mySize.x * 0.5f <= myPosition.x + myHalfWidth * 2.0f);
	const bool insideY = (aObject.myPosition.y + aObject.mySize.y * 0.5f >= myPosition.y) && (aObject.myPosition.y - aObject.mySize.y * 0.5f <= myPosition.y + myHalfWidth * 2.0f);

	return (insideX && insideY);
}

bool Border::ContainsPoint(Vector2<float> aPosition) const
{
	const Vector2f MinPoint = { myPosition.x - mySize.x, myPosition.y - mySize.y };
	const Vector2f MaxPoint = { myPosition.x + mySize.x, myPosition.y + mySize.y };
	if (aPosition.x >= MinPoint.x && aPosition.x <= MaxPoint.x)
	{
		if (aPosition.y >= MinPoint.y && aPosition.y <= MaxPoint.y)
		{
			return true;
		}
	}
	return false;
}