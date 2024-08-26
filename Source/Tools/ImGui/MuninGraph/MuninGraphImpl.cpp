#include "MuninGraphImpl.h"

#include <stdexcept>

#include "NodeGraph/UID.h"
#include "Types/TypeRegistry.h"

bool MuninGraph::Initialize()
{
	if(!TypeRegistry::Get().Initialize())
		return false;

	if(myRegisteredNodeClasses.empty())
	{
		myRegisteredNodeClasses.reserve(myNodeRegistrations.size());
		for(const auto& regFunc : myNodeRegistrations)
		{
			RegisteredNodeClass nodeClass = regFunc();
			myNodeTypeToIndex.emplace(nodeClass.Type, myRegisteredNodeClasses.size());

			myNodeClassNameToIndex.emplace(nodeClass.TypeName, myRegisteredNodeClasses.size());

			myNodeFriendlyNameToIndex.emplace(nodeClass.Name, myRegisteredNodeClasses.size());
			std::vector<std::type_index> supportedNodeSchemas = nodeClass.SupportedSchemas;
			myRegisteredNodeClasses.emplace_back(std::move(nodeClass));

			for(const auto& schema : supportedNodeSchemas)
			{
				if(mySchemaTypeToNode.contains(schema))
				{
					std::vector<const RegisteredNodeClass*>& schemaClassList = mySchemaTypeToNode.at(schema);
					RegisteredNodeClass* ptr = &myRegisteredNodeClasses.back();
					schemaClassList.push_back(ptr);
				}
				else
				{
					RegisteredNodeClass* ptr = &myRegisteredNodeClasses.back();
					std::vector<const RegisteredNodeClass*> nodeClassList = { ptr };
					mySchemaTypeToNode.emplace(schema, std::move(nodeClassList));
				}
			}			
		}
	}

	return !myRegisteredNodeClasses.empty();
}

bool MuninGraph::IsNodeSupportedBySchema(std::type_index aSchemaType, std::type_index aNodeType) const
{
	if(myNodeTypeToIndex.contains(aNodeType))
	{
		const size_t index = myNodeTypeToIndex.at(aNodeType);
		const RegisteredNodeClass& nodeClass = myRegisteredNodeClasses.at(index);
		for(const std::type_index& t : nodeClass.SupportedSchemas)
		{
			if(t == aSchemaType)
				return true;
		}
	}

	return false;
}

const std::vector<const RegisteredNodeClass*>& MuninGraph::GetSupportedNodeClasses(std::type_index aSchemaType) const
{
	if(!mySchemaTypeToNode.contains(aSchemaType))
	{
		throw std::runtime_error("That schema does not exist!");
	}

	return mySchemaTypeToNode.at(aSchemaType);
}

const RegisteredNodeClass& MuninGraph::GetNodeClass(std::type_index aNodeType) const
{
	if(!myNodeTypeToIndex.contains(aNodeType))
	{
		throw std::out_of_range("Could not find the requested node type!");
	}

	return myRegisteredNodeClasses.at(myNodeTypeToIndex.at(aNodeType));
}

const RegisteredNodeClass& MuninGraph::GetNodeClass(std::string_view aNodeType) const
{
	if(const auto it = myNodeClassNameToIndex.find(aNodeType); it != myNodeClassNameToIndex.end())
	{
		return myRegisteredNodeClasses.at(it->second);
	}

	throw std::out_of_range("Could not find the requested node type!");	
}

bool MuninGraph::RegisterNodeClass(const RegisterNodeClassFunc& aFunc)
{
	myNodeRegistrations.emplace_back(aFunc);
	return true;
}
