#pragma once
#include <typeindex>
#include <vector>

#include "GlobalUID.h"
#ifndef FORCEINLINE
#define FORCEINLINE __forceinline
#endif

enum class PinDirection
{
	Input,
	Output
};



// POD class representing a Pin.
template<typename NodeClass, typename GraphSchemaType>
class NodeGraphPin : public Munin::ObjectGUID<NodeGraphPin<NodeClass, GraphSchemaType>>
{
	friend GraphSchemaType;

	std::string myLabel;
	PinDirection myDirection;

	std::shared_ptr<NodeClass> myOwner;

	std::vector<size_t> myEdges;

protected:

	// When we're moved we don't want to create a new GlobalUID.
	//NodeGraphPin(NodeGraphPin&& other) noexcept
	//	: Munin::GlobalUID<NodeGraphPin<NodeClass>>(std::move(other))
	//{  }

	NodeGraphPin(NodeGraphPin&& other) noexcept = default;

	// Only other nodes may initialize us. This is to force init required members.
	NodeGraphPin(const std::shared_ptr<NodeClass>& anOwner, const std::string& aLabel, PinDirection aDirection)
		: myLabel(aLabel), myDirection(aDirection), myOwner(anOwner)
	{  }
	
public:

	virtual ~NodeGraphPin() = default;

	FORCEINLINE std::shared_ptr<NodeClass> GetOwner() const { return myOwner; }

	const FORCEINLINE std::vector<size_t>& GetEdges() const { return myEdges; } 

	virtual FORCEINLINE bool IsPinConnected() const { return !myEdges.empty(); }
	virtual void AddPinEdge(size_t anEdgeUID);
	virtual void RemovePinEdge(size_t anEdgeUID);
	size_t FORCEINLINE GetNumConnections() const { return myEdges.size(); }

	virtual FORCEINLINE PinDirection GetPinDirection() const { return myDirection; }
	virtual FORCEINLINE const std::string& GetLabel() const { return myLabel; }

	virtual FORCEINLINE GraphColor GetColor() const { return GraphColor::White; }

	void SetLabel(const std::string& aLabel) { myLabel = aLabel; }
};

template <typename NodeClass, typename GraphSchemaType>
void NodeGraphPin<NodeClass, GraphSchemaType>::AddPinEdge(size_t anEdgeUID)
{
	myEdges.push_back(anEdgeUID);
}

template <typename NodeClass, typename GraphSchemaType>
void NodeGraphPin<NodeClass, GraphSchemaType>::RemovePinEdge(size_t anEdgeUID)
{
	for(auto it = myEdges.begin(); it != myEdges.end(); ++it)
	{
		if(*it == anEdgeUID)
		{
			it = myEdges.erase(it);
			break;
		}
	}
}