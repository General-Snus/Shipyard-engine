#include "PersistentSystems.pch.h"

#include "WorldGraph.h"
#include "Tools/Utilities/DataStructures/QuadTree.h"

void WorldGraph::InitializeWorld()
{
	//WorldGrid = new QuadTree<Vector3f>(0, 0, 100);
}

void WorldGraph::Update()
{
	if (ShouldUpdateGrid())
	{
		WorldGrid.Add<Vector3f>(Vector3f(0, 0, 0), nullptr);
		//WorldGrid.update();
	}
}

bool WorldGraph::ShouldUpdateGrid()
{
	return true;
}
