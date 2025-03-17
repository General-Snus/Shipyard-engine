
#include "PersistentSystems.pch.h"

#include "../NetworkStructs.h"
#include "../NetMessage/NetMessage.h"
#include <Networking/NetMessage/StringMessages.h>
#include <WinSock2.h>
#include <string.h> 

#pragma optimize( "", off ) 

using enum SessionConfiguration::HostType;
using enum NetworkConnection::Status;



NetworkConnection::Status NetworkConnection::StartAsServer(NetAddress serverAddress,SessionConfiguration cfg)
{
	connectedTo = serverAddress;

	Close();

	if(OpenSockets(cfg) == failed)
	{
		return failed;
	}

	sockaddr  server = connectedTo.as_sockaddr();

	//We bind both udp and tcp
	if(bind(TCPSocket,&server,sizeof(server)) == SOCKET_ERROR)
	{
		LOGGER.ErrC("Failed to bind TCP: ",WSAGetLastError());
		Close();
		status = failed;
		return status;
	}
	if(bind(UDPSocket,&server,sizeof(server)) == SOCKET_ERROR)
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
	connectedTo = serverAddress;

	status = StartAsClient(connectedTo,cfg);
	if(status != Status::initializedAsClient)
	{
		status = StartAsServer(connectedTo,cfg);
	}

	return  status;
}

NetworkConnection::Status NetworkConnection::StartAsClient(NetAddress serverAddress,SessionConfiguration cfg)
{
	connectedTo = serverAddress;
	Close();
	if(OpenSockets(cfg) == failed)
	{
		return failed;
	}

	sockaddr  server = connectedTo.as_sockaddr();
	if(connect(TCPSocket,&server,sizeof(server)) == SOCKET_ERROR)
	{
		LOGGER.Err("Could not connect to server: " + WSAGetLastError());
		Close();
		status = failed;
		return status;
	}

	return status = Status::initializedAsClient;;
}

NetworkConnection::Status NetworkConnection::StartAsRemoteConnection(NetAddress remote,NetworkSocket socket)
{
	Close();

	//TODO check that accept socket status was all gut
	TCPSocket = socket;
	UDPSocket = INVALID_SOCKET;
	status = Status::initialized;
	connectedTo = remote;
	return status;
}

NetworkConnection::Status NetworkConnection::OpenSockets(SessionConfiguration cfg)
{
	cfg;

	TCPSocket = socket(AF_INET,SOCK_STREAM,0);
	UDPSocket = socket(AF_INET,SOCK_DGRAM,0);

	if(TCPSocket == INVALID_SOCKET || UDPSocket == INVALID_SOCKET)
	{
		LOGGER.Err("Could not create socket: " + WSAGetLastError());
		return Status::failed;
	}

	return Status::initialized;
}

bool NetworkConnection::Send(const NetMessage& message,Protocol protocol) const
{
	switch(protocol)
	{
	case NetworkConnection::Protocol::TCP:
		return NetworkConnection::TCP(TCPSocket,message);
		break;
	case NetworkConnection::Protocol::UDP:
		return NetworkConnection::UDP(connectedTo,UDPSocket,message);
		break;
	default:
		std::unreachable();
		break;
	}
}

bool NetworkConnection::Send(const NetMessage& message,NetAddress target,Protocol protocol) const
{
	switch(protocol)
	{
	case NetworkConnection::Protocol::TCP:
		return NetworkConnection::TCP(TCPSocket,message);
		break;
	case NetworkConnection::Protocol::UDP:
		return NetworkConnection::UDP(target,UDPSocket,message);
		break;
	default:
		std::unreachable();
		break;
	}
}

bool NetworkConnection::SendUDP(NetAddress toAddress,const NetMessage& message) const
{
	return NetworkConnection::UDP(toAddress,UDPSocket,message);
}

bool NetworkConnection::TCP(NetworkSocket throughSocket,const NetMessage& message)
{
	if(send(throughSocket,(const char*)&message,sizeof(message),0) == SOCKET_ERROR)
	{
		LOGGER.ErrC("TCP SEND FAILED: ",errno);
		LOGGER.ErrC("TCP SEND FAILED: ",WSAGetLastError());
		return false;
	}
	return true;
}

bool NetworkConnection::UDP(NetAddress toAddress,NetworkSocket throughSocket,const NetMessage& message)
{
	auto addr = toAddress.as_sockaddr();
	if(sendto(throughSocket,(const char*)&message,sizeof(message),0,&addr,sizeof(addr)) == SOCKET_ERROR)
	{
		LOGGER.ErrC("UDP SEND FAILED: ",errno);
		LOGGER.ErrC("UDP SEND FAILED: ",WSAGetLastError());
		return false;
	}
	return true;
}

NetworkConnection::Status NetworkConnection::GetStatus() const
{
	return status;
}

NetAddress NetworkConnection::Address() const
{
	return connectedTo;
}

bool NetworkConnection::Close()
{
	bool ret = closesocket(TCPSocket) == 0 && closesocket(UDPSocket) == 0;
	TCPSocket = 0;
	UDPSocket = 0;
	return ret;
}

bool NetworkConnection::ReceiveUDP(NetMessage* message,NetAddress* receivedFrom,const int bufferSize,bool blocking,const float msTimeout) const
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

	sockaddr_in other{};
	int         length = sizeof(other);

	u_long iMode = !blocking;
	auto iResult = ioctlsocket(UDPSocket,FIONBIO,&iMode);
	if(iResult != NO_ERROR)
	{
		return false;
	}

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

	auto receivedLength = recvfrom(UDPSocket,(char*)message,bufferSize,0,(sockaddr*)&other,&length);
	if(receivedLength == SOCKET_ERROR || receivedLength > MAX_NETMESSAGE_SIZE)
	{
		//LOGGER.ErrC("Failed to recieve message ",WSAGetLastError());
		auto socketInfo = NetworkHelpers::GetSocketSettings(TCPSocket);
		return false;
	}
	else
	{
		receivedFrom->address = other.sin_addr.S_un.S_addr;
		receivedFrom->port = other.sin_port;
		return true;
	}
}

bool NetworkConnection::ReceiveTCP(NetMessage* message,NetAddress* receivedFrom,const int bufferSize,bool blocking,const float msTimeout) const
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
		return false;
	}
	else
	{
		*receivedFrom = connectedTo;
		return true;
	}
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
		return false;
	}
	else
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


NetAddress::NetAddress() : NetAddress(LOCALHOST,DEFAULT_PORT) {}

NetAddress::NetAddress(const std::string& Ip,unsigned short port,AddressFamily family) : family(family)
{
	if(inet_pton(AF_INET,Ip.c_str(),&this->address) != 1)
	{
		throw std::exception("Bad ip");
	}

	this->port = htons(port);
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
		}
		else
		{
			settings.receiveBufferSize = optionValue;
		}

		// Retrieve the send buffer size (SO_RCVBUF)
		if(getsockopt(sock,SOL_SOCKET,SO_RCVBUF,(char*)&optionValue,&optionLength) == SOCKET_ERROR)
		{
			std::cerr << "Failed to get SO_SNDBUF: " << WSAGetLastError() << std::endl;
		}
		else
		{
			settings.sendBufferSize = optionValue;
		}

		// Retrieve whether address reuse is enabled (SO_REUSEADDR)
		if(getsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(char*)&optionValue,&optionLength) == SOCKET_ERROR)
		{
			std::cerr << "Failed to get SO_REUSEADDR: " << WSAGetLastError() << std::endl;
		}
		else
		{
			settings.reuseAddress = (optionValue != 0);
		}
		u_long mode = 0;
		// Retrieve socket blocking mode (SO_BLOCKING) - usually 1 for blocking, 0 for non-blocking
		if(ioctlsocket(sock,FIONBIO,&mode) == SOCKET_ERROR)
		{
			std::cerr << "Failed to get SO_BLOCKING: " << WSAGetLastError() << std::endl;
		}
		else
		{
			settings.isBlocking = (mode == 0);
		}

		// Retrieve TCP_NODELAY (for TCP sockets)
		if(getsockopt(sock,IPPROTO_TCP,TCP_NODELAY,(char*)&optionValue,&optionLength) == SOCKET_ERROR)
		{
			std::cerr << "Failed to get TCP_NODELAY: " << WSAGetLastError() << std::endl;
		}
		else
		{
			settings.tcpNoDelay = (optionValue != 0);
		}

		// Retrieve maximum segment size for TCP (TCP_MAXSEG)
		if(getsockopt(sock,IPPROTO_TCP,TCP_MAXSEG,(char*)&optionValue,&optionLength) == SOCKET_ERROR)
		{
			std::cerr << "Failed to get TCP_MAXSEG: " << WSAGetLastError() << std::endl;
		}
		else
		{
			settings.maxSegmentSize = optionValue;
		}

		// Retrieve TCP KeepAlive options
		// KeepAlive Idle time (TCP_KEEPIDLE)
		if(getsockopt(sock,IPPROTO_TCP,TCP_KEEPIDLE,(char*)&optionValue,&optionLength) == SOCKET_ERROR)
		{
			std::cerr << "Failed to get TCP_KEEPIDLE: " << WSAGetLastError() << std::endl;
		}
		else
		{
			settings.keepIdle = optionValue;
		}

		// KeepAlive Interval time (TCP_KEEPINTVL)
		if(getsockopt(sock,IPPROTO_TCP,TCP_KEEPINTVL,(char*)&optionValue,&optionLength) == SOCKET_ERROR)
		{
			std::cerr << "Failed to get TCP_KEEPINTVL: " << WSAGetLastError() << std::endl;
		}
		else
		{
			settings.keepInterval = optionValue;
		}

		// KeepAlive count (TCP_KEEPCNT)
		if(getsockopt(sock,IPPROTO_TCP,TCP_KEEPCNT,(char*)&optionValue,&optionLength) == SOCKET_ERROR)
		{
			std::cerr << "Failed to get TCP_KEEPCNT: " << WSAGetLastError() << std::endl;
		}
		else
		{
			settings.keepCount = optionValue;
		}

		return settings;
	}
}

#pragma optimize( "", on )

NetworkedId NetworkedId::Generate()
{
	return NetworkedId(uuid::v4::UUID::New());

}
NetworkedId::NetworkedId() = default;
