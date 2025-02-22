#pragma once
#include <Windows.h>
#include <string>
#include <array> 

struct NetworkedId {
    unsigned short id = 0;
};

enum class eNetMessageType : unsigned char {
    None,
    ChatMessage,
    Handshake,
    Quit,
    PlayerJoin,
    PlayerSync,
    Count
}; 

constexpr size_t MAX_NETMESSAGE_SIZE = 512;
constexpr size_t NETMESSAGE_HEADERSIZE = sizeof(eNetMessageType) + sizeof(NetworkedId) + sizeof(NetworkedId) + sizeof(NetworkedId);
constexpr size_t NETMESSAGE_BUFFERSIZE = (MAX_NETMESSAGE_SIZE - NETMESSAGE_HEADERSIZE);

#undef GetMessage
#undef GetMessageW




//Recieve directly into NetMessage address please..

class NetMessage
{
    friend struct NetworkConnection; 
  public:
    NetMessage() = default;
    ~NetMessage() = default;

    char *GetBuffer()
    {
        return dataBuffer.data();
    }
    unsigned char GetId()
    {
        return  myId;
    } 
    void SetId(unsigned char aId)
    {
        myId = aId;
    }
    eNetMessageType myType = eNetMessageType::None;

  protected:
    unsigned char myId = {};
    NetworkedId mySenderId = {};
    NetworkedId myTargetId = {};

    std::array<char,NETMESSAGE_BUFFERSIZE> dataBuffer{}; // Use NETMESSAGE_BUFFERSIZE
};
