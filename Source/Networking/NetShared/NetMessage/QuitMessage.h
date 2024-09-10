#pragma once
#include "NetMessage.h"

class QuitMessage : public NetMessage
{
public:
	QuitMessage() { myType = eNetMessageType::Quit; }

	void SetMessage(const std::string& aMessage)
	{
		memcpy(myBuffer, aMessage.data(), sizeof(char) * aMessage.size());
	}

	std::string ReadMessage()
	{
		std::string output = myBuffer;

		const size_t position = output.find('\n');
		output = output.substr(0, position);

		return output;
	}

private:

};
