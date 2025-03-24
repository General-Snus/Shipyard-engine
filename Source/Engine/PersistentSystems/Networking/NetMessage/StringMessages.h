#pragma once
#include "NetMessage.h"

class StringMessage : public NetMessage
{
public:

	void SetMessage(const std::string& aMessage)
	{
		memcpy(dataBuffer.data(),aMessage.data(),sizeof(char) * aMessage.size());
	}

	std::string ReadMessage() const
	{
		std::string output = dataBuffer.data();

		const size_t position = output.find('\n');
		output = output.substr(0,position);

		return output;
	}

	constexpr static eNetMessageType type = eNetMessageType::None;
protected:
	explicit StringMessage(NetworkedId id) : NetMessage(id) {}
	StringMessage() = default;
};

class HandshakeMessage : public StringMessage
{
public:
	explicit HandshakeMessage(NetworkedId id) : StringMessage(id) { myType = type; }
	explicit HandshakeMessage() { myType = type; }
	constexpr static eNetMessageType type = eNetMessageType::NewConnection; 
};

class NicknameMessage : public StringMessage
{
public:
	explicit NicknameMessage(NetworkedId id) : StringMessage(id) { myType = type; }
	explicit NicknameMessage() { myType = type; }
	constexpr static eNetMessageType type = eNetMessageType::NicknameSet;
};


class ChatMessage : public StringMessage
{
public:
	explicit ChatMessage(NetworkedId id) : StringMessage(id) { myType = type; }
	explicit ChatMessage() { myType = type; }
	constexpr static eNetMessageType type = eNetMessageType::ChatMessage;
};

class QuitMessage : public StringMessage
{
public:
	QuitMessage() { myType = type; }
	constexpr static eNetMessageType type = eNetMessageType::Quit;
};
