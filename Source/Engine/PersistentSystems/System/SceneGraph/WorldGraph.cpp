#include "PersistentSystems.pch.h"

#include "WorldGraph.h"
#include "Tools/Utilities/System/Grid.h"

void WorldGraph::InitializeWorld()
{
	WorldGrid = new Grid2D<Vector3f>(0, 0, 100);
}

void WorldGraph::Update()
{
	if (ShouldUpdateGrid())
	{
		WorldGrid->update();
	}
}

bool WorldGraph::ShouldUpdateGrid()
{
	return true;
}
