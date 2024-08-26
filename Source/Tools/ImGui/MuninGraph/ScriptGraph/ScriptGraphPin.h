#pragma once
#include "ScriptGraphCommon.h"
#include "NodeGraph/NodeGraphPin.h"
#include "Types/TypeRegistry.h"

class ScriptGraphSchema;
class ScriptGraphNode;

class ScriptGraphPin : public NodeGraphPin<ScriptGraphPin, ScriptGraphNode, ScriptGraphSchema>
{	
public:

	FORCEINLINE ScriptGraphPinType GetPinType() const { return myPinType; }
	FORCEINLINE bool IsLabelVisible() const { return myLabelVisible; }
	FORCEINLINE bool IsDynamicPin() const { return myIsDynamicPin; }
	FORCEINLINE const RegisteredType* GetPinDataType() const { return myData.GetType(); }
	FORCEINLINE const TypedDataContainer& GetDataContainer() const { return myData; }
	FORCEINLINE TypedDataContainer& GetMutableDataContainer() { return myData; }

	static ScriptGraphPin CreatePin(ScriptGraphNode* aOwner, std::string_view aLabel, ScriptGraphPinType aType, PinDirection aDirection, bool aHideLabelOnNode = false);
	static ScriptGraphPin CreateDataPin(ScriptGraphNode* aOwner, std::string_view aLabel, ScriptGraphPinType aType, PinDirection aDirection, std::type_index aDataType, bool aHideLabelOnNode = false);
	static ScriptGraphPin CreateDynamicPin(ScriptGraphNode* aOwner, std::string_view aLabel, ScriptGraphPinType aType, PinDirection aDirection);
	static ScriptGraphPin CreateDynamicDataPin(ScriptGraphNode* aOwner, std::string_view aLabel, ScriptGraphPinType aType, PinDirection aDirection, std::type_index aDataType);

	// No copying Pins
	ScriptGraphPin(const ScriptGraphPin&) = delete;
	ScriptGraphPin& operator=(const ScriptGraphPin&) = delete;

	ScriptGraphPin(ScriptGraphPin&&) = default;
	ScriptGraphPin& operator=(ScriptGraphPin&&) = default;	

	template<typename T>
	bool TryGetValue(T& outValue) const
	{
		return myData.TryGet(outValue);
	}

	template<typename T>
	bool TrySetValue(T& aValue) const
	{
		return myData.TrySet(aValue);
	}

	bool TryChangeDataType(const RegisteredType* aType);

private:

	// Only the Schema can create pins.
	friend ScriptGraphSchema;

	ScriptGraphPin(ScriptGraphNode* aOwner, std::string_view aLabel, PinDirection aDirection, ScriptGraphPinType aType)
		: NodeGraphPin(aOwner, aLabel, aDirection), myPinType(aType)
	{  }

	ScriptGraphPin(ScriptGraphNode* aOwner, std::string_view aLabel, PinDirection aDirection, ScriptGraphPinType aType, std::type_index aDataType);

	TypedDataContainer myData;
	ScriptGraphPinType myPinType;
	bool myLabelVisible = true;
	bool myIsDynamicPin = false;

};
