#pragma once
#include <functional>
#include <memory>
#include <string>
#include <typeindex>

#include "Internal/MuninGraphCommon.h"

enum NodeClassFlags
{
	CF_None					= 0x00000000u,
	CF_Abstract				= 0x00000001u,
	CF_Internal				= 0x00000002u,
	// CF_Next				= 0x00000004u,8u, 10u, 20u, 40u, etc
};

struct RegisteredNodeClass
{
	std::type_index Type = typeid(std::nullptr_t);
	std::type_index Base = typeid(std::nullptr_t);
	int Flags = CF_None;
	std::function<std::shared_ptr<void>()> New;

	std::shared_ptr<void> ClassDefaultObject;

	std::string Name;
	std::string TypeName;
	std::string Category;

	std::vector<std::type_index> SupportedSchemas;

	FORCEINLINE bool HasFlag(NodeClassFlags aFlag) const { return (Flags & aFlag); }

	template<typename T>
	FORCEINLINE std::shared_ptr<T> GetCDO() const
	{
		return std::static_pointer_cast<T>(ClassDefaultObject);
	}
};

class MuninGraph
{
	friend class GraphSchemaBase;

	template<class NodeClass, NodeClassFlags>
	friend struct NodeClassAutoRegister;

public:

	static MuninGraph& Get()
	{
		static MuninGraph thisInstance;
		return thisInstance;
	}

	MuninGraph(const MuninGraph&) = delete;
	MuninGraph(MuninGraph&&) noexcept = delete;
	MuninGraph& operator=(const MuninGraph&) = delete;
	MuninGraph& operator=(MuninGraph&&) noexcept = delete;

	bool Initialize();

	bool IsNodeSupportedBySchema(std::type_index aSchemaType, std::type_index aNodeType) const;

	// Finds all supported node classes for a particular schema.
	const std::vector<const RegisteredNodeClass*>& GetSupportedNodeClasses(std::type_index aSchemaType) const;

	const RegisteredNodeClass& GetNodeClass(std::type_index aNodeType) const;
	const RegisteredNodeClass& GetNodeClass(std::string_view aNodeType) const;

private:

	typedef std::function<RegisteredNodeClass()> RegisterNodeClassFunc;

	std::vector<RegisterNodeClassFunc> myNodeRegistrations;
	std::vector<RegisteredNodeClass> myRegisteredNodeClasses;

	// TypeId to Node Index. Nodes are never removed from the Registered Node Classes list anyway.
	std::unordered_map<std::type_index, size_t> myNodeTypeToIndex;
	std::unordered_map<std::string, size_t, string_hash, std::equal_to<>> myNodeFriendlyNameToIndex;
	std::unordered_map<std::string, size_t, string_hash, std::equal_to<>> myNodeClassNameToIndex;

	// Acceleration map for finding supported nodes per schema in a quick way.
	std::unordered_map<std::type_index, std::vector<const RegisteredNodeClass*>> mySchemaTypeToNode;

	MuninGraph() = default;
	~MuninGraph() = default;

	bool RegisterNodeClass(const RegisterNodeClassFunc& aFunc);
};