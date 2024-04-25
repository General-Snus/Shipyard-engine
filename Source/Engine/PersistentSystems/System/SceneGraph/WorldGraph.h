#pragma once
class Grid;

class WorldGraph
{
public:
	static void InitializeWorld();
private:	
	inline static Grid* WorldGrid;
};

