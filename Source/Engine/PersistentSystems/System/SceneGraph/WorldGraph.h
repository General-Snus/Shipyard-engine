#pragma once
#include "Tools/Utilities/System/Grid.h"
#include "Tools/Utilities/LinearAlgebra/Vector3.hpp"

class WorldGraph
{
public:
	static void InitializeWorld();

	static void Update();
private:
	static bool ShouldUpdateGrid();
	static Grid2D<Vector3f>* WorldGrid;
};

