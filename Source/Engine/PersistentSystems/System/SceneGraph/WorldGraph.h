#pragma once
#include "Tools/Utilities/DataStructures/VoxelGrid.h"
#include "Tools/Utilities/LinearAlgebra/Vector3.hpp"

class WorldGraph
{
public:
	static void InitializeWorld();

	static void Update();
private:
	static bool ShouldUpdateGrid();
	static SpacialHashGrid<100,1,100> WorldGrid;
};

