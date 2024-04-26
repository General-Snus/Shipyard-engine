#pragma once
class Grid;

class WorldGraph
{
public:
	static void InitializeWorld();

	static void Update();
private:
	static bool ShouldUpdateGrid();
	inline static Grid* WorldGrid;
};

