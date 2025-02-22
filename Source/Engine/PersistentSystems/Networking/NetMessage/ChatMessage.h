#pragma once
#include "NetMessage.h"

class ChatMessage : public NetMessage
{
public:
	ChatMessage() { myType = eNetMessageType::ChatMessage; }

	void SetMessage(const std::string& aMessage)
	{
		memcpy(dataBuffer.data(),aMessage.data(),sizeof(char) * aMessage.size());
	}

	std::string ReadMessage()
	{
		std::string output = dataBuffer.data();

		const size_t position = output.find('\n');
		output = output.substr(0, position);

		return output;
	} 
};
