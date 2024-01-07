#pragma once
#include "GraphColor.h"

struct NodeGraphEdge
{
	size_t EdgeId;
	size_t FromUID;
	size_t ToUID;

	GraphColor Color;
	float Thickness = 1.0f;
};
