#pragma once
#include <iostream>
#include <thread> 
#include "Engine/PersistentSystems/Networking/NetMessage/NetMessage.h"
#include "Engine/PersistentSystems/Networking/NetworkStructs.h" 
#pragma comment(lib, "Ws2_32.lib")
#undef max

class Client
{
  public:
    Client();
    ~Client();

    void Setup();
    void Update();
    void Close();

    bool HasReceivedMessage() const;
    eNetMessageType GetType();
    NetMessage*GetLatestMessage();
    template <typename T> T *GetLatestMessageAsType();

    bool SetupConnection();
    bool SendOK();
    void InputIpAndPort(std::string &outIp, short &outPort);
    void MessageLoop();

    void Send(const std::string &aMessage);

    template <class T, typename Data> void Send(const Data &someData);

  private:
    NetworkConnection connection; 
    NetMessage* buffer;
    bool hasRecievedMessage; 
    NetAddress serverAddress;

    unsigned char myId{}; 

    std::thread receiveThread;
    std::thread sendThread;
};

template <class T, typename Data> void Client::Send(const Data &someData)
{
    T message;
    message.SetMessage(someData);
    message.SetId(myId);
    connection.Send(message,serverAddress);
}

template <typename T> T *Client::GetLatestMessageAsType()
{
    return reinterpret_cast<T *>(buffer);
}
