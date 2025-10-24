#pragma once
#include "NetMessage.h"
#include "Tools\Utilities\LinearAlgebra\Quaternions.hpp"
#include "Tools\Utilities\Input\Mapper.hpp"
#include <cassert>
#include "cstring" 
#include "Tools\Utilities\LinearAlgebra\Vector3.hpp"

struct TransformSyncData
{
	NetworkedId uniqueComponentId;
	Vector3<float> myPosition{};
	Quaternionf    myQuaternion{};
	Vector3<float> myScale = Vector3f(1,1,1);
};

class TransformSyncMessage: public NetMessage
{
public:
	TransformSyncMessage()
	{
		myType = type;
	}

	void SetMessage(const TransformSyncData &someData)
	{
		static_assert(NETMESSAGE_BUFFERSIZE > sizeof(someData));
		memcpy(&dataBuffer,&someData,sizeof(someData));
	}

	TransformSyncData ReadMessage() const
	{
		TransformSyncData data;
		memcpy(&data,&dataBuffer,sizeof(data));
		return data;
	}

	constexpr static eNetMessageType type = eNetMessageType::TransformSyncMessage;
private:
};


struct GameobjectInformation
{
	NetworkedId uniqueComponentId;
	std::vector<std::string> listOfComponentsNames; // Extract string name here
};


class CreateObjectMessage: public NetMessage
{
public:
	CreateObjectMessage()
	{
		myType = type;
	}

	void SetMessage(const GameobjectInformation &someData)
	{
		static_assert(NETMESSAGE_BUFFERSIZE > sizeof(someData));

		auto addr = dataBuffer.data();

		memcpy(addr,&someData.uniqueComponentId,sizeof(someData.uniqueComponentId));

		addr += sizeof(someData.uniqueComponentId);


		std::string concatedNames;
		for(auto& i : someData.listOfComponentsNames)
		{
			if(i.size() +1 + concatedNames.size() + sizeof(someData.uniqueComponentId)>  NETMESSAGE_BUFFERSIZE)
			{
				break;
			}
			concatedNames += i;
			concatedNames += ":";
		}

		concatedNames.copy(addr,concatedNames.size());
	}

	GameobjectInformation ReadMessage() const
	{
		GameobjectInformation data;
		memcpy(&data,&dataBuffer,sizeof(data.uniqueComponentId));
		auto addr = dataBuffer.data() + sizeof(data.uniqueComponentId) ;
		std::string componentsName(addr);

		for(auto& i : componentsName)
		{
			i;

		}

		return data;
	}

	constexpr static eNetMessageType type = eNetMessageType::CreateObjectMessage;
private:
};

class DestroyObjectMessage: public NetMessage
{
public:
	DestroyObjectMessage()
	{
		myType = type;
	}

	void SetMessage(const NetworkedId &someData)
	{
		static_assert(NETMESSAGE_BUFFERSIZE > sizeof(someData));
		memcpy(&dataBuffer,&someData,sizeof(someData));
	}

	NetworkedId ReadMessage() const
	{
		NetworkedId data;
		memcpy(&data,&dataBuffer,sizeof(data));
		return data;
	}

	constexpr static eNetMessageType type = eNetMessageType::DestroyObjectMessage;
private:
};
 
class RegisterPlayerMessage : public NetMessage
{
public:
	RegisterPlayerMessage()
	{
		myType = type;
	}

	void SetMessage(const Networking::AreaOfInterest& someData)
	{
		static_assert(NETMESSAGE_BUFFERSIZE > sizeof(someData));

		auto addr = dataBuffer.data();

		memcpy(addr, &someData, sizeof(someData));

		addr += sizeof(someData);
		 
	}

	Networking::AreaOfInterest ReadMessage() const
	{
		Networking::AreaOfInterest data;
		memcpy(&data, &dataBuffer, sizeof(data)); 
		return data;
	}

	constexpr static eNetMessageType type = eNetMessageType::PlayerRegistered;
private:
};


class InputEventMessage : public NetMessage
{
	
public:
	struct InputEventMessageData {
		Action action;
		NetworkedId object;
	};

	InputEventMessage()
	{
		myType = type;
	}

	void SetMessage(const InputEventMessageData& someData)
	{
		static_assert(NETMESSAGE_BUFFERSIZE > sizeof(someData));

		auto addr = dataBuffer.data();

		memcpy(addr, &someData, sizeof(someData));

		addr += sizeof(someData);

	}

	InputEventMessageData ReadMessage() const
	{
		InputEventMessageData data;
		memcpy(&data, &dataBuffer, sizeof(data));
		return data;
	}

	constexpr static eNetMessageType type = eNetMessageType::InputEventMessage;
private:
};

