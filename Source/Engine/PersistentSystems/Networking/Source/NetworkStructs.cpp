
#include "PersistentSystems.pch.h"

#include "../NetworkStructs.h"
#include "../NetMessage/NetMessage.h"
#include <Networking/NetMessage/StringMessages.h>
#include <WinSock2.h>
#include <string.h>  
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <icmpapi.h>

#pragma comment(lib, "Ws2_32.lib")

#pragma optimize( "", off ) 

using enum SessionConfiguration::HostType;
using enum NetworkConnection::Status;

using namespace NetworkHelpers;

bool IsSocketBound(NetworkSocket socket) {
	sockaddr_in localAddr;
	int addrLen = sizeof(localAddr);

	// Attempt to get the local address and port
	if(getsockname(socket,(sockaddr*)&localAddr,&addrLen) == SOCKET_ERROR) {
		LOGGER.ErrC("getsockname failed ",GetWSAErrorString(WSAGetLastError()));
		return false;
	}

	// Check if the port is non-zero, indicating the socket is bound
	if(ntohs(localAddr.sin_port) != 0) {
		LOGGER.LogC("Socket is bound to port ",ntohs(localAddr.sin_port));
		return true;
	} else {
		LOGGER.Log("Socket is not bound to port ");
		return false;
	}
}



NetworkConnection::Status NetworkConnection::StartAsServer(NetAddress serverAddress,SessionConfiguration cfg)
{
	tcpConnectionAddress = serverAddress;

	Close();

	if(OpenSockets(cfg) == failed)
	{
		return failed;
	}

	sockaddr  server = tcpConnectionAddress.as_sockaddr();

	//We bind both udp and tcp
	if(bind(TCPSocket,&server,sizeof(server)) == SOCKET_ERROR)
	{
		LOGGER.ErrC("Failed to bind TCP: ",WSAGetLastError());
		Close();
		status = failed;
		return status;
	}

	BOOL optval = TRUE;
	auto result = setsockopt(UDPSocket,SOL_SOCKET,SO_REUSEADDR,(const char*)&optval,sizeof(optval));
	if(result == SOCKET_ERROR)
	{
		LOGGER.ErrC("Failed to set sockopt to reuse: ",WSAGetLastError());
		Close();
		status = failed;
		return status;
	}

	sockaddr_in udpServerAddr{};
	udpServerAddr.sin_family = AF_INET;
	udpServerAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);;
	udpServerAddr.sin_port = htons(DEFAULT_PORT);

	if(bind(UDPSocket,(sockaddr*)&udpServerAddr,sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		LOGGER.ErrC("Failed to bind UPD: ",WSAGetLastError());
		Close();
		status = failed;
		return status;
	}

	//UDP is connectionless so no listen
	if(listen(TCPSocket,SOMAXCONN) == SOCKET_ERROR)
	{
		LOGGER.ErrC("Failed to listen on TCP socket: ",WSAGetLastError());
		Close();
		status = failed;
		return status;
	}

	status = Status::initializedAsServer;
	return status;
}

NetworkConnection::Status NetworkConnection::AutoHostOrClient(NetAddress serverAddress,SessionConfiguration cfg)
{
	tcpConnectionAddress = serverAddress;

	status = StartAsClient(tcpConnectionAddress,cfg);
	if(status != Status::initializedAsClient)
	{
		status = StartAsServer(tcpConnectionAddress,cfg);
	}

	return  status;
}

NetworkConnection::Status NetworkConnection::StartAsClient(NetAddress serverAddress,SessionConfiguration cfg)
{
	tcpConnectionAddress = serverAddress;
	Close();
	if(OpenSockets(cfg) == failed)
	{
		return failed;
	}


	sockaddr  server = tcpConnectionAddress.as_sockaddr();
	if(connect(TCPSocket,&server,sizeof(server)) == SOCKET_ERROR)
	{
		LOGGER.WarnC("Could not connect to server: ",GetWSAErrorString(WSAGetLastError()));
		Close();
		status = failed;
		return status;
	}

	//sockaddr_in udpClientAddr = NetAddress().as_sockaddr_in(); 
	//if(bind(UDPSocket,(sockaddr*)&udpClientAddr,sizeof(udpClientAddr)) == SOCKET_ERROR) {
	//	LOGGER.ErrC("Failed to bind UDP: ",GetWSAErrorString(WSAGetLastError()));
	//	Close();
	//	status = failed;
	//	return status;
	//}

	return status = Status::initializedAsClient;;
}

NetworkConnection::Status NetworkConnection::StartAsRemoteConnection(NetAddress remote,NetworkSocket socket)
{
	Close();

	//TODO check that accept socket status was all gut
	TCPSocket = socket;
	UDPSocket = INVALID_SOCKET;
	status = Status::initialized;
	tcpConnectionAddress = remote;
	return status;
}




NetworkConnection::Status NetworkConnection::OpenSockets(SessionConfiguration cfg)
{
	cfg;

	int iResult;

	struct addrinfo* result = NULL;
	struct addrinfo hints;

	ZeroMemory(&hints,sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	hints.ai_flags = AI_PASSIVE;
	auto t = cfg.address;
	iResult = getaddrinfo(LOCALHOST,std::format("{}",DEFAULT_PORT).c_str(),&hints,&result);

	UDPSocket = socket(hints.ai_family,hints.ai_socktype,hints.ai_protocol);
	TCPSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	UDPSocket = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if(TCPSocket == INVALID_SOCKET || UDPSocket == INVALID_SOCKET)
	{
		LOGGER.ErrC("Could not create socket: ",GetWSAErrorString(WSAGetLastError()));
		return Status::failed;
	}

	return Status::initialized;
}

bool NetworkConnection::Accept(NetworkConnection* returningConnection) const
{
	assert(returningConnection);
	if(!returningConnection)
	{
		return  false;
	}

	sockaddr addr{};
	int length = sizeof(sockaddr);
	NetworkSocket socket = accept(TCPSocket,&addr,&length);
	if(socket == INVALID_SOCKET)
	{
		LOGGER.ErrC("Failed to accept socket ",GetWSAErrorString(WSAGetLastError()));
		return false;
	} else
	{
		sockaddr_in address = *(sockaddr_in*)&addr;
		NetAddress netAddr;
		netAddr.family = (NetAddress::AddressFamily)address.sin_family;
		netAddr.address = address.sin_addr.S_un.S_addr;
		netAddr.port = address.sin_port;

		returningConnection->StartAsRemoteConnection(netAddr,socket);
		return true;

	}
}

NetAddress NetworkConnection::Address() const
{
	return tcpConnectionAddress;
}

bool NetworkConnection::Close()
{
	bool tcpClosed = false;
	bool udpClosed = false;
	if(TCPSocket != INVALID_SOCKET)
	{
		shutdown(TCPSocket,SD_BOTH);
		tcpClosed = closesocket(TCPSocket) == 0;
		TCPSocket = INVALID_SOCKET;
	}

	if(UDPSocket != INVALID_SOCKET)
	{
		udpClosed = closesocket(UDPSocket) == 0;
		UDPSocket = INVALID_SOCKET;
	}
	return tcpClosed && udpClosed;
}

NetworkConnection::Status NetworkConnection::GetStatus() const
{
	return status;
}


bool NetworkConnection::TCP(NetworkSocket throughSocket,  NetMessage& message)
{
	message.timePoint = std::chrono::high_resolution_clock::now();
	if(send(throughSocket,(const char*)&message,sizeof(message),0) == SOCKET_ERROR)
	{
		LOGGER.ErrC("TCP SEND FAILED: ",errno);
		LOGGER.ErrC("TCP SEND FAILED: ",GetWSAErrorString(WSAGetLastError()));
		return false;
	}
	return true;
}

bool NetworkConnection::SendUDP(NetAddress toAddress,  NetMessage& message) const
{
	return NetworkConnection::UDP(toAddress,UDPSocket,message);
}

bool NetworkConnection::SendTCP(  NetMessage & message) const
{
	return NetworkConnection::TCP(TCPSocket,message);
}

bool NetworkConnection::UDP(NetAddress toAddress,NetworkSocket throughSocket,  NetMessage& message)
{
	message.timePoint = std::chrono::high_resolution_clock::now();
	auto addr = toAddress.as_sockaddr();
	if(sendto(throughSocket,(const char*)&message,sizeof(message),0,&addr,sizeof(addr)) == SOCKET_ERROR)
	{
		LOGGER.ErrC("UDP SEND FAILED: ",errno);
		LOGGER.ErrC("UDP SEND FAILED: ",GetWSAErrorString(WSAGetLastError()));
		return false;
	}
	return true;
}

bool NetworkConnection::ReceiveUDP(NetMessage* message,NetAddress* receivedFrom,const int bufferSize,bool blocking,const float msTimeout) const
{
	blocking;
	assert(bufferSize <= 512);

	if(status && ~Status::initialized)
	{
		throw std::overflow_error("Is not initialized correctly");
	}

	if(bufferSize > 512)
	{
		throw	std::overflow_error("To large buffer, max size is 512");
	}

	if(message->GetBuffer() == nullptr || receivedFrom == nullptr)
	{
		throw	std::bad_alloc();
	}

	sockaddr_in other{};
	int         length = sizeof(other);

	/*u_long iMode = !blocking;
	auto iResult = ioctlsocket(UDPSocket,FIONBIO,&iMode);
	if(iResult != NO_ERROR)
	{
		return false;
	}*/

	if(msTimeout > 0)
	{
		struct timeval tv;
		tv.tv_sec = (long)(msTimeout / 1000.f);
		tv.tv_usec = (long)msTimeout;

		// Set the receive timeout
		if(setsockopt(UDPSocket,SOL_SOCKET,SO_RCVTIMEO,(const char*)&tv,sizeof(tv)) < 0)
		{
			perror("Error setting socket timeout");
		}
	}
	auto receivedLength = recvfrom(UDPSocket,(char*)message,1024,0,(sockaddr*)&other,&length);
	if(receivedLength == SOCKET_ERROR || receivedLength > MAX_NETMESSAGE_SIZE)
	{
		return false;
	} else
	{
		receivedFrom->address = other.sin_addr.S_un.S_addr;
		receivedFrom->port = other.sin_port;
		return true;
	}
}

int NetworkConnection::ReceiveTCP(NetMessage* message,NetAddress* receivedFrom,const int bufferSize,bool blocking,const float msTimeout) const
{
	assert(bufferSize <= 512);

	if(status && ~Status::initialized)
	{
		throw std::overflow_error("Is not initialized correctly");
	}

	if(bufferSize > 512)
	{
		throw	std::overflow_error("To large buffer, max size is 512");
	}

	if(message->GetBuffer() == nullptr || receivedFrom == nullptr)
	{
		throw	std::bad_alloc();
	}
	blocking;
	// If iMode = 0, blocking is enabled; 
	// If iMode != 0, non-blocking mode is enabled. 
	//u_long iMode = !blocking;
	//auto iResult = ioctlsocket(TCPSocket,FIONBIO,&iMode);
	//if(iResult != NO_ERROR) {
	//	throw std::exception("Cant really fail tbh");
	//}

	if(msTimeout > 0)
	{
		struct timeval tv;
		tv.tv_sec = (long)(msTimeout / 1000.f);
		tv.tv_usec = (long)msTimeout;

		// Set the receive timeout
		if(setsockopt(TCPSocket,SOL_SOCKET,SO_RCVTIMEO,(const char*)&tv,sizeof(tv)) < 0)
		{
			throw std::exception("Shit went bad when setting timeout");
		}
	}

	auto receivedLength = recv(TCPSocket,(char*)message,bufferSize,0);
	if(receivedLength == SOCKET_ERROR || receivedLength > MAX_NETMESSAGE_SIZE)
	{
		LOGGER.ErrC("Failed to recieve TCP message ",GetWSAErrorString(WSAGetLastError()));
		auto socketInfo = NetworkHelpers::GetSocketSettings(TCPSocket);socketInfo;
		return WSAGetLastError();
	} else
	{
		*receivedFrom = tcpConnectionAddress;
		return 0;
	}
}

NetAddress::NetAddress(): NetAddress(LOCALHOST,DEFAULT_PORT) {}
NetAddress::NetAddress(const std::string& Ip,unsigned short hostshortPort,AddressFamily family): family(family)
{
	if(inet_pton(AF_INET,Ip.c_str(),&this->address) != 1)
	{
		throw std::exception("Bad ip");
	}

	this->port = htons(hostshortPort); // returns a netshort
}
std::string NetAddress::IPStr() const
{
	std::string out;
	switch(family)
	{
	case AddressFamily::ipv4:
		out.resize(INET_ADDRSTRLEN);
		inet_ntop(AF_INET,&address,out.data(),INET_ADDRSTRLEN);
		break;

	case AddressFamily::ipv6:
		out.resize(INET6_ADDRSTRLEN);
		inet_ntop(AF_INET6,&address,out.data(),INET6_ADDRSTRLEN);
		break;
	}

	return out;
}
sockaddr_in NetAddress::as_sockaddr_in() const
{
	sockaddr_in intermediate{};

	intermediate.sin_family = AF_INET;
	intermediate.sin_port = port;
	intermediate.sin_addr.S_un.S_addr = address;

	return intermediate;
}
sockaddr NetAddress::as_sockaddr() const
{
	return std::bit_cast<sockaddr>(as_sockaddr_in());
}

namespace NetworkHelpers
{
SocketSettings GetSocketSettings(unsigned long long sock)
{
	SocketSettings settings = {0};
	int optionValue;
	int optionLength = sizeof(optionValue);

	// Retrieve the receive buffer size (SO_RCVBUF)
	if(getsockopt(sock,SOL_SOCKET,SO_RCVBUF,(char*)&optionValue,&optionLength) == SOCKET_ERROR)
	{
		std::cerr << "Failed to get SO_RCVBUF: " << WSAGetLastError() << std::endl;
	} else
	{
		settings.receiveBufferSize = optionValue;
	}

	// Retrieve the send buffer size (SO_RCVBUF)
	if(getsockopt(sock,SOL_SOCKET,SO_RCVBUF,(char*)&optionValue,&optionLength) == SOCKET_ERROR)
	{
		std::cerr << "Failed to get SO_SNDBUF: " << WSAGetLastError() << std::endl;
	} else
	{
		settings.sendBufferSize = optionValue;
	}

	// Retrieve whether address reuse is enabled (SO_REUSEADDR)
	if(getsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(char*)&optionValue,&optionLength) == SOCKET_ERROR)
	{
		std::cerr << "Failed to get SO_REUSEADDR: " << WSAGetLastError() << std::endl;
	} else
	{
		settings.reuseAddress = (optionValue != 0);
	}
	u_long mode = 0;
	// Retrieve socket blocking mode (SO_BLOCKING) - usually 1 for blocking, 0 for non-blocking
	if(ioctlsocket(sock,FIONBIO,&mode) == SOCKET_ERROR)
	{
		std::cerr << "Failed to get SO_BLOCKING: " << WSAGetLastError() << std::endl;
	} else
	{
		settings.isBlocking = (mode == 0);
	}

	// Retrieve TCP_NODELAY (for TCP sockets)
	if(getsockopt(sock,IPPROTO_TCP,TCP_NODELAY,(char*)&optionValue,&optionLength) == SOCKET_ERROR)
	{
		std::cerr << "Failed to get TCP_NODELAY: " << WSAGetLastError() << std::endl;
	} else
	{
		settings.tcpNoDelay = (optionValue != 0);
	}

	// Retrieve maximum segment size for TCP (TCP_MAXSEG)
	if(getsockopt(sock,IPPROTO_TCP,TCP_MAXSEG,(char*)&optionValue,&optionLength) == SOCKET_ERROR)
	{
		std::cerr << "Failed to get TCP_MAXSEG: " << WSAGetLastError() << std::endl;
	} else
	{
		settings.maxSegmentSize = optionValue;
	}

	// Retrieve TCP KeepAlive options
	// KeepAlive Idle time (TCP_KEEPIDLE)
	if(getsockopt(sock,IPPROTO_TCP,TCP_KEEPIDLE,(char*)&optionValue,&optionLength) == SOCKET_ERROR)
	{
		std::cerr << "Failed to get TCP_KEEPIDLE: " << WSAGetLastError() << std::endl;
	} else
	{
		settings.keepIdle = optionValue;
	}

	// KeepAlive Interval time (TCP_KEEPINTVL)
	if(getsockopt(sock,IPPROTO_TCP,TCP_KEEPINTVL,(char*)&optionValue,&optionLength) == SOCKET_ERROR)
	{
		std::cerr << "Failed to get TCP_KEEPINTVL: " << WSAGetLastError() << std::endl;
	} else
	{
		settings.keepInterval = optionValue;
	}

	// KeepAlive count (TCP_KEEPCNT)
	if(getsockopt(sock,IPPROTO_TCP,TCP_KEEPCNT,(char*)&optionValue,&optionLength) == SOCKET_ERROR)
	{
		std::cerr << "Failed to get TCP_KEEPCNT: " << WSAGetLastError() << std::endl;
	} else
	{
		settings.keepCount = optionValue;
	}

	return settings;
}
std::string GetWSAErrorString(int error)
{
	switch(error)
	{
		CASE(WSABASEERR)
			CASE(WSAEINTR)
			CASE(WSAEBADF)
			CASE(WSAEACCES)
			CASE(WSAEFAULT)
			CASE(WSAEINVAL)
			CASE(WSAEMFILE)
			CASE(WSAEWOULDBLOCK)
			CASE(WSAEINPROGRESS)
			CASE(WSAEALREADY)
			CASE(WSAENOTSOCK)
			CASE(WSAEDESTADDRREQ)
			CASE(WSAEMSGSIZE)
			CASE(WSAEPROTOTYPE)
			CASE(WSAENOPROTOOPT)
			CASE(WSAEPROTONOSUPPORT)
			CASE(WSAESOCKTNOSUPPORT)
			CASE(WSAEOPNOTSUPP)
			CASE(WSAEPFNOSUPPORT)
			CASE(WSAEAFNOSUPPORT)
			CASE(WSAEADDRINUSE)
			CASE(WSAEADDRNOTAVAIL)
			CASE(WSAENETDOWN)
			CASE(WSAENETUNREACH)
			CASE(WSAENETRESET)
			CASE(WSAECONNABORTED)
			CASE(WSAECONNRESET)
			CASE(WSAENOBUFS)
			CASE(WSAEISCONN)
			CASE(WSAENOTCONN)
			CASE(WSAESHUTDOWN)
			CASE(WSAETOOMANYREFS)
			CASE(WSAETIMEDOUT)
			CASE(WSAECONNREFUSED)
			CASE(WSAELOOP)
			CASE(WSAENAMETOOLONG)
			CASE(WSAEHOSTDOWN)
			CASE(WSAEHOSTUNREACH)
			CASE(WSAENOTEMPTY)
			CASE(WSAEPROCLIM)
			CASE(WSAEUSERS)
			CASE(WSAEDQUOT)
			CASE(WSAESTALE)
			CASE(WSAEREMOTE)
			CASE(WSAEDISCON)
			CASE(WSASYSNOTREADY)
			CASE(WSAVERNOTSUPPORTED)
			CASE(WSANOTINITIALISED)
			CASE(WSAHOST_NOT_FOUND)
			CASE(WSATRY_AGAIN)
			CASE(WSANO_RECOVERY)
			CASE(WSANO_DATA)
	default:
		{

			return std::format("Not a Winsock error (%d)",error);;
		}
	}
}
}

#pragma optimize( "", on )

NetworkedId NetworkedId::Generate()
{
	return NetworkedId(uuid::v4::UUID::New());

}

NetworkedId::NetworkedId(): id(uuid::v4::UUID::New()) {};
