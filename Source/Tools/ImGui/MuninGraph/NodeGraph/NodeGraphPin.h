#pragma once
#include <cassert>
#include <string>
#include <vector>

#include "NodeGraphCommon.h"

template<typename NodePinClass, typename NodeClass, typename GraphSchemaClass>
class NodeGraphPin : public TypeObjectUID<NodePinClass>
{
	// Schema can do whatever it wants to us.
	friend GraphSchemaClass;

	PinDirection myDirection;
	NodeClass* myOwner;

	std::string myLabel;
	std::vector<size_t> myEdges;

protected:

	NodeGraphPin(NodeClass* aOwner, std::string_view aLabel, PinDirection aDirection)
		: myDirection(aDirection), myOwner(aOwner), myLabel(aLabel)
	{
		assert(aOwner != nullptr);
	}

	NodeGraphPin(NodeGraphPin&& other) noexcept = default;

public:

	~NodeGraphPin() override
	{
		myOwner = nullptr;
		myLabel = "";
		myEdges.clear();
	}

	virtual void AddPinEdge(size_t aEdgeUID)
	{
		myEdges.push_back(aEdgeUID);
	}

	virtual void RemovePinEdge(size_t aEdgeUID)
	{
		std::erase_if(myEdges, [aEdgeUID](size_t v){ return v == aEdgeUID; });
	}

	FORCEINLINE NodeClass* GetOwner() const { return myOwner; }
	FORCEINLINE const std::vector<size_t>& GetEdges() const { return myEdges; }
	FORCEINLINE PinDirection GetPinDirection() const { return myDirection; }
	FORCEINLINE bool IsConnected() const { return !myEdges.empty(); }
	FORCEINLINE size_t GetNumConnections() const { return myEdges.size(); }

	virtual FORCEINLINE std::string_view GetLabel() const { return myLabel; }

	void SetLabel(std::string_view aNewLabel) { myLabel = aNewLabel.data(); }
};

