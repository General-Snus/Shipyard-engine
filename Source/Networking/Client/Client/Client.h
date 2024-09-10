#pragma once
#include <iostream>
#include <thread>

#include <WS2tcpip.h>

#include "Networking/NetShared/NetMessage/NetMessage.h"
#pragma comment(lib, "Ws2_32.lib")
#undef max

#define DEFAULT_PORT 27015
#define DEFAULT_IP "127.0.0.1"

class Client
{
  public:
    Client();
    ~Client();

    void Setup();
    void Update();
    void Close();

    bool HasRecievedMessage();
    eNetMessageType GetType();
    char *GetLatestMessage();
    template <typename T> T *GetLatestMessageAsType();

    bool SetupConnection();
    bool SendOK();
    void InputIpAndPort(std::string &outIp, short &outPort);
    void MessageLoop();

    void Send(const std::string &aMessage);

    template <class T, typename Data> void Send(const Data &someData);

  private:
    char inbound[MAX_NETMESSAGE_SIZE];
    bool hasRecievedMessage;

    SOCKET mySocket;
    sockaddr_in myClient;
    int myLength;
    unsigned char myId{};
    bool myExit{};

    std::thread receiveThread;
    std::thread sendThread;
};

template <class T, typename Data> void Client::Send(const Data &someData)
{
    T message;
    message.SetMessage(someData);
    message.SetId(myId);

    char outbound[MAX_NETMESSAGE_SIZE];
    memcpy(outbound, &message, sizeof(T));

    if (sendto(mySocket, outbound, MAX_NETMESSAGE_SIZE, 0, reinterpret_cast<sockaddr *>(&myClient), sizeof(myClient)) ==
        SOCKET_ERROR)
    {
        std::cout << "Failed to send" << std::endl;
    }
}
template <typename T> T *Client::GetLatestMessageAsType()
{
    return reinterpret_cast<T *>(&inbound);
}
