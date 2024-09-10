#pragma once
#include <Windows.h>
#include <string>

#define MAX_NETMESSAGE_SIZE 512
#define NETMESSAGE_HEADERSIZE 8

#undef GetMessage
#undef GetMessageW

enum class eNetMessageType : unsigned char
{
    None,
    ChatMessage,
    Handshake,
    Quit,
    PlayerJoin,
    PlayerSync,
    Count
};

class NetMessage
{
  protected:
    eNetMessageType myType = eNetMessageType::None;
    unsigned char myId = 0;
    unsigned short mySenderId = 0;
    unsigned short myTargetId = 0;

    char myBuffer[MAX_NETMESSAGE_SIZE - NETMESSAGE_HEADERSIZE]{};

  public:
    NetMessage() = default;
    ~NetMessage() = default;

    char *GetBuffer()
    {
        return myBuffer;
    }
    unsigned char GetId()
    {
        return myId;
    }

    void SetId(unsigned char aId)
    {
        myId = aId;
    }
};
