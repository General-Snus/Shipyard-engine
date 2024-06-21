#include "Engine\PersistentSystems\PersistentSystems.pch.h"
#include "WorldGraph.h"

#include "Tools/Utilities/System/Grid.h"

void WorldGraph::InitializeWorld()
{
	WorldGrid = new Grid(0,0,100);
}

void WorldGraph::Update()
{
	if (ShouldUpdateGrid())
	{
		WorldGrid->Update();
	}
}

bool WorldGraph::ShouldUpdateGrid()
{
	return true;
}
