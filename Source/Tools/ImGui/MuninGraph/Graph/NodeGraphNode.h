#pragma once
#include <algorithm>
#include <cassert>
#include <memory>
#include <unordered_map>
#include <vector>

#include "NodeGraphPin.h"
#include "NodeGraph.h"

#ifndef FORCEINLINE
#define FORCEINLINE __forceinline
#endif

// Represents a connection between two nodes. Connections are always Forward
// which means that the StartPinUID is on the SOURCE node and the EndPinUID
// is on the TARGET node.

template<typename GraphPinType, typename GraphType, typename GraphSchemaType>
class NodeGraphNode
{
	friend GraphSchemaType;

	// Acceleration map to find pins based on UID;
	std::unordered_map<size_t, GraphPinType> myPins;
	std::unordered_map<std::string, size_t> myPinLabelToUID;

	std::shared_ptr<GraphType> myOwner = nullptr;

	float myPosition[3] = { 0, 0, 0 };

protected:

	void AddPin(GraphPinType&& aPin)
	{
		myPinLabelToUID.insert({ aPin.GetLabel(), aPin.GetUID() });
		myPins.insert({ aPin.GetUID(), std::move(aPin) });		
	}
	
	template<typename T>
	bool GetPinData(const std::string& aPinLabel, T& outData) const
	{
		const GraphPinType& myPin = GetPin(aPinLabel);
		if(myPin.IsPinConnected())
		{
			bool sourceErrored = false;
			const GraphPinType& dataPin = myOwner->GetDataSourcePin(myPin.GetUID(), sourceErrored);
			if(!sourceErrored)
			{
				return dataPin.GetData(outData);
			}

			return false;
		}
		return myPin.GetData(outData);
	}

	bool GetRawPinData(const std::string& aPinLabel, void* outData, size_t outDataSize) const;

	bool SetRawPinData(const std::string& aPinLabel, const void* inData, size_t aDataSize);

	template<typename T>
	void SetPinData(const std::string& aPinLabel, const T& aData)
	{
		const auto It = myPinLabelToUID.find(aPinLabel);
		assert(It != myPinLabelToUID.end() && "That pin could not be found!");
		GraphPinType& pin = myPins.find(It->second)->second;
		pin.SetData(aData);
	}

	bool RenamePin(const std::string& aPinLabel, const std::string& aNewName);
	bool RenamePin(size_t aPinUID, const std::string& aNewName);

	bool IsPinUIDLabel(size_t aPinUId, const std::string& aPinLabel) const;

public:

	static const typename GraphType::SupportedNodeClass& GetClass();

	virtual void Init() = 0;

	const std::shared_ptr<GraphType> GetOwner() const { return myOwner;  }

	virtual ~NodeGraphNode();

	virtual FORCEINLINE std::string GetNodeTitle() const { return "Graph Node"; }
	virtual FORCEINLINE std::string GetNodeCategory() const { return "Default"; }

	const FORCEINLINE std::unordered_map<size_t, GraphPinType>& GetPins() const { return myPins; }
	const GraphPinType& GetPin(size_t aPinUID) const;
	const GraphPinType& GetPin(const std::string& aPinLabel) const;
	size_t GetPinLabelUID(const std::string& aPinLabel) const;

	// These are here because ImNodeEd has its own position property.
	void UpdateNodePositionCache(float x, float y, float z);
	// These are here because ImNodeEd has its own position property.
	void GetNodePositionCache(float& outX, float& outY, float& outZ) const;

	/**
	 * Called when this node is asked to perform whatever operation it's supposed to do.
	 * @returns The Pin Index of the pin we want to exit on after performing our operation or 0 if we don't have a pin to exit on.
	 */
	//virtual size_t DoOperation() { return Exit(); }
};

template<typename GraphPinType, typename GraphType, typename GraphSchemaType>
bool NodeGraphNode<GraphPinType, GraphType, GraphSchemaType>::GetRawPinData(const std::string& aPinLabel,
	void* outData, size_t outDataSize) const
{
	const GraphPinType& myPin = GetPin(aPinLabel);
	if (myPin.IsPinConnected())
	{
		bool sourceErrored = false;
		const GraphPinType& dataPin = myOwner->GetDataSourcePin(myPin.GetUID(), sourceErrored);
		return dataPin.GetRawData(outData, outDataSize);
	}
	return myPin.GetRawData(outData, outDataSize);
}

template<typename GraphPinType, typename GraphType, typename GraphSchemaType>
bool NodeGraphNode<GraphPinType, GraphType, GraphSchemaType>::SetRawPinData(const std::string& aPinLabel,
	const void* inData, size_t aDataSize)
{
	const auto It = myPinLabelToUID.find(aPinLabel);
	assert(It != myPinLabelToUID.end() && "That pin could not be found!");
	GraphPinType& pin = myPins.find(It->second)->second;
	pin.SetRawData(inData, aDataSize);
	return true;
}

//template<typename GraphPinType, typename GraphType, typename GraphSchemaType>
//size_t NodeGraphNode<GraphPinType, GraphType, GraphSchemaType>::ExitViaPin(const std::string& aPinLabel)
//{
//	auto It = myPinLabelToUID.find(aPinLabel);
//	assert(It != myPinLabelToUID.end() && "That pin could not be found!");
//	return It->second;
//}

template<typename GraphPinType, typename GraphType, typename GraphSchemaType>
bool NodeGraphNode<GraphPinType, GraphType, GraphSchemaType>::RenamePin(const std::string& aPinLabel,
	const std::string& aNewName)
{
	if(const auto pinNameIt = myPinLabelToUID.find(aPinLabel); pinNameIt != myPinLabelToUID.end())
	{
		if(auto pinIt = myPins.find(pinNameIt->second); pinIt != myPins.end())
		{
			myPinLabelToUID.erase(pinIt->second.GetLabel());
			pinIt->second.SetLabel(aNewName);
			myPinLabelToUID.insert({ aNewName, pinIt->second.GetUID() });
			return true;
		}
	}

	return false;
}

template<typename GraphPinType, typename GraphType, typename GraphSchemaType>
bool NodeGraphNode<GraphPinType, GraphType, GraphSchemaType>::RenamePin(size_t aPinUID, const std::string& aNewName)
{
	if (auto pinIt = myPins.find(aPinUID); pinIt != myPins.end())
	{
		myPinLabelToUID.erase(pinIt->second.GetLabel());
		pinIt->second.SetLabel(aNewName);
		myPinLabelToUID.insert({ aNewName, pinIt->second.GetUID() });
		return true;
	}

	return false;
}

template <typename GraphPinType, typename GraphType, typename GraphSchemaType>
bool NodeGraphNode<GraphPinType, GraphType, GraphSchemaType>::IsPinUIDLabel(size_t aPinUId,
	const std::string& aPinLabel) const
{
	if(const auto pinId = myPinLabelToUID.find(aPinLabel); pinId != myPinLabelToUID.end())
	{
		return pinId->second == aPinUId;
	}

	return false;
}

template <typename GraphPinType, typename GraphType, typename GraphSchemaType>
const typename GraphType::SupportedNodeClass& NodeGraphNode<GraphPinType, GraphType, GraphSchemaType>::GetClass()
{
	return GraphType::GetSupportedNodeClass(typeid(*this));
}

template<typename GraphPinType, typename GraphType, typename GraphSchemaType>
inline NodeGraphNode<GraphPinType, GraphType, GraphSchemaType>::~NodeGraphNode()
{
}

template<typename GraphPinType, typename GraphType, typename GraphSchemaType>
const GraphPinType& NodeGraphNode<GraphPinType, GraphType, GraphSchemaType>::GetPin(size_t aPinUID) const
{
	auto It = myPins.find(aPinUID);
	assert(It != myPins.end() && "That pin could not be found!");
	return It->second;
}

template<typename GraphPinType, typename GraphType, typename GraphSchemaType>
const GraphPinType& NodeGraphNode<GraphPinType, GraphType, GraphSchemaType>::GetPin(const std::string& aPinLabel) const
{
	const auto It = myPinLabelToUID.find(aPinLabel);
	assert(It != myPinLabelToUID.end() && "That pin could not be found!");
	return myPins.find(It->second)->second;
}

template <typename GraphPinType, typename GraphType, typename GraphSchemaType>
size_t NodeGraphNode<GraphPinType, GraphType, GraphSchemaType>::GetPinLabelUID(const std::string& aPinLabel) const
{
	const auto It = myPinLabelToUID.find(aPinLabel);
	assert(It != myPinLabelToUID.end() && "That pin could not be found!");
	return It->second;
}

template <typename GraphPinType, typename GraphType, typename GraphSchemaType>
void NodeGraphNode<GraphPinType, GraphType, GraphSchemaType>::UpdateNodePositionCache(float x, float y, float z)
{
	myPosition[0] = x;
	myPosition[1] = y;
	myPosition[2] = z;
}

template <typename GraphPinType, typename GraphType, typename GraphSchemaType>
void NodeGraphNode<GraphPinType, GraphType, GraphSchemaType>::GetNodePositionCache(float& outX, float& outY,
	float& outZ) const
{
	outX = myPosition[0];
	outY = myPosition[1];
	outZ = myPosition[2];
}

template<typename GraphType, typename NodeType, typename BaseType, unsigned NodeFlags, unsigned UserFlags>
class zz_AutoRegisterGraphNode
{
	static inline bool IsRegistered = GraphType::template RegisterNodeTypeWithBase<NodeType, BaseType>(NodeFlags, UserFlags);
};

//~ Begin Munin macro shenanigans

// Microsofts old preprocessor has its own ideas of how things should work.
#if !defined(_MSVC_TRADITIONAL) || _MSVC_TRADITIONAL
// Logic using the traditional preprocessor (/Zc:preprocessor-)
#define PP_COUNT_ARGS_N(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define PP_COUNT_ARGS(...) PP_EXPAND(PP_COUNT_ARGS_N(__VA_ARGS__, _10, _9, _8, _7, _6, _5, _4, _3, _2, _1, _0))
#define PP_EXPAND(...) __VA_ARGS__

#define PP_SELECT_FUNC_EXPAND(MFunc,NumArgs) MFunc ## NumArgs
#define PP_SELECT_FUNC(MFunc,NumArgs) PP_SELECT_FUNC_EXPAND(MFunc, NumArgs)
#define PP_OVERLOAD_MACRO(MFunc, ...) PP_SELECT_FUNC(MFunc, PP_COUNT_ARGS(, __VA_ARGS__))(__VA_ARGS__)
#else
// Logic using cross-platform compatible preprocessor (/Zc:preprocessor)
#define PP_SELECT_FUNC_EXPAND(MFunc,NumArgs) MFunc ## NumArgs
#define PP_EXPAND(...) __VA_ARGS__

// Macro overloading feature support
#define PP_VA_ARG_SIZE(...) PP_EXPAND(PP_APPLY_ARG_N((PP_ZERO_ARGS_DETECT(__VA_ARGS__), PP_RSEQ_N)))

#define PP_ZERO_ARGS_DETECT(...) PP_EXPAND(PP_ZERO_ARGS_DETECT_PREFIX_ ## __VA_ARGS__ ## _ZERO_ARGS_DETECT_SUFFIX)
#define PP_ZERO_ARGS_DETECT_PREFIX__ZERO_ARGS_DETECT_SUFFIX ,,,,,,,,,,,0

#define PP_APPLY_ARG_N(ARGS) PP_EXPAND(PP_ARG_N ARGS)
#define PP_ARG_N(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N,...) N
#define PP_RSEQ_N 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define PP_SELECT_FUNC(MFunc, NumArgs) PP_SELECT_FUNC_EXPAND( MFunc ## _, NumArgs)
#define PP_OVERLOAD_MACRO(MFunc, ...) PP_SELECT_FUNC(MFunc, PP_VA_ARG_SIZE(__VA_ARGS__))(__VA_ARGS__)
#endif

// Examples:
// #define testMacro_3(A, B, C) int abc
// #define testMacro_2(A, B) int ab
// #define testMacro_1(A) int a
// #define testMacro_0() bool no
// #define testMacro(...) PP_OVERLOAD_MACRO(testMacro, __VA_ARGS__)
// Allows access by writing testMacro(a, b, c) etc.

//~ End Munin macro shenanigans

#define AutoRegisterGraphNode_3(GraphType, NodeType, NodeBaseType) class NodeType : public NodeBaseType, public zz_AutoRegisterGraphNode<GraphType, NodeType, NodeBaseType, 0, 0>, public Munin::ObjectGUID<NodeType>
#define AutoRegisterGraphNode_4(GraphType, NodeType, NodeBaseType, NodeFlags) class NodeType : public NodeBaseType, public zz_AutoRegisterGraphNode<GraphType, NodeType, NodeBaseType, NodeFlags, 0>, public Munin::ObjectGUID<NodeType>
#define AutoRegisterGraphNode_5(GraphType, NodeType, NodeBaseType, NodeFlags, UserFlags) class NodeType : public NodeBaseType, public zz_AutoRegisterGraphNode<GraphType, NodeType, NodeBaseType, NodeFlags, UserFlags>, public Munin::ObjectGUID<NodeType>

/*
 * Declares a new Node Class
 * Example:
 * AutoRegisterGraphNode(MyGraphClass, MyNodeClassName, MyNodeParentClass)
 * AutoRegisterGraphNode(MyGraphClass, MyNodeClassName, MyNodeParentClass, SomeNodeFlags)
 * AutoRegisterGraphNode(MyGraphClass, MyNodeClassName, MyNodeParentClass, SomeNodeFlags, SomeUserFlags)
 */
#define AutoRegisterGraphNode(...) PP_OVERLOAD_MACRO(AutoRegisterGraphNode, __VA_ARGS__)