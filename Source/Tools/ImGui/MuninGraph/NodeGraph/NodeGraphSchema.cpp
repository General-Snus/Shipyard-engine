#include "NodeGraphSchema.h"

#include "MuninGraph.h"

GraphSchemaBase::GraphSchemaBase(std::type_index aMyType)
	: mySchemaType(aMyType)
{  }

GraphSchemaBase::~GraphSchemaBase()
{  }

bool GraphSchemaBase::IsNodeSupportedBySchema(std::type_index aNodeType) const
{
	const MuninGraph& muninGraph = MuninGraph::Get();
	return muninGraph.IsNodeSupportedBySchema(mySchemaType, aNodeType);
}
