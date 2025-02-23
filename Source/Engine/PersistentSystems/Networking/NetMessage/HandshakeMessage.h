#pragma once
#include "NetMessage.h"

class HandshakeMessage : public NetMessage
{
public:
	HandshakeMessage() { myType = eNetMessageType::Handshake; }

	void SetMessage(const std::string& aMessage)
	{
		memcpy(dataBuffer.data(), aMessage.data(), sizeof(char) * aMessage.size());
	}

	std::string ReadMessage()
	{
		std::string output = dataBuffer.data();

		const size_t position = output.find('\n');
		output = output.substr(0, position);

		return output;
	}

	constexpr static eNetMessageType type = eNetMessageType::Handshake;
private:
};