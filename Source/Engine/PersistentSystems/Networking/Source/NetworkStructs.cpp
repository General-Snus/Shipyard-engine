#include "PersistentSystems.pch.h"

#include "../NetworkStructs.h"
#include "../NetMessage/NetMessage.h"


NetworkConnection::Status NetworkConnection::StartAsServer(NetAddress serverAddress,int socketType,int socketProtocol,unsigned short bindType) {
	connectedTo = serverAddress;
	if(Socket != INVALID_SOCKET) {
		closesocket(Socket);
	}

	Socket = socket(bindType,socketType,socketProtocol); // SOCK_DGRAM = UDP

	if(Socket == INVALID_SOCKET) {
		std::cerr << "Could not create socket: " << WSAGetLastError() << "\n";
		status = Status::failed;
		return status;
	}

	sockaddr_in server{};
	server.sin_family = bindType;
	server.sin_port = connectedTo.port;
	server.sin_addr.S_un.S_addr = connectedTo.address;

	if(bind(Socket,(sockaddr*)&server,sizeof(server)) == SOCKET_ERROR) {
		std::cerr << "Could not connect to server: " << WSAGetLastError() << "\n";
		status = Status::failed;
		Socket = INVALID_SOCKET;
		return status;
	}

	if(listen(Socket,SOMAXCONN) == SOCKET_ERROR) {
		std::cerr << "Could not listen on socket: " << WSAGetLastError() << "\n";
		closesocket(Socket);
		status = Status::failed;
		Socket = INVALID_SOCKET;
		return status;
	}

	status = Status::initialized; 
	return status;
}

NetworkConnection::Status NetworkConnection::AutoHostOrClient(NetAddress serverAddress,int socketType,int socketProtocol,unsigned short bindType) { 
	connectedTo = serverAddress;
	return StartAsClient(connectedTo,socketType,socketProtocol,bindType) == NetworkConnection::Status::initialized ? NetworkConnection::Status::initialized : StartAsServer(connectedTo,socketType,socketProtocol,bindType);
}

NetworkConnection::Status NetworkConnection::StartAsClient(NetAddress serverAddress,int socketType,int socketProtocol,unsigned short bindType) {
	connectedTo = serverAddress;
	if(Socket != INVALID_SOCKET) {
		closesocket(Socket);
	}

	Socket = socket(bindType,socketType,socketProtocol); // SOCK_DGRAM = UDP

	if(Socket == INVALID_SOCKET) {
		std::cerr << "Could not create socket: " << WSAGetLastError() << "\n";
		status = Status::failed;
		return status;
	}

	sockaddr_in server{};
	server.sin_family = bindType;
	server.sin_port = connectedTo.port;
	server.sin_addr.S_un.S_addr = connectedTo.address;

	if(connect(Socket,(sockaddr*)&server,sizeof(server)) == SOCKET_ERROR) {
		std::cerr << "Could not connect to server: " << WSAGetLastError() << "\n";
		status = Status::failed;
		Socket = INVALID_SOCKET;
		return status;
	}

	status = Status::initialized;
	return status;
}

bool NetworkConnection::Send(const NetMessage& message) const {
	sockaddr addr = connectedTo.asAddress();
	int length = sizeof(addr);

	if(sendto(Socket,(const char*)&message,sizeof(message),0,&addr,length) == SOCKET_ERROR) {
		return false; 
	}
	return true;
}

bool NetworkConnection::Close() const {
	return closesocket(Socket) == 0;
}

bool NetworkConnection::Receive(NetMessage* message,NetAddress* receivedFrom,const int bufferSize,const float timeout) const {
	UNREFERENCED_PARAMETER(timeout); 
	assert(bufferSize <= 512);

	if(status != Status::initialized) {
		throw std::overflow_error("Is not initialized correctly");
	}

	if(bufferSize > 512) {
		throw	std::overflow_error("To large buffer, max size is 512");
	}

	if(message->GetBuffer() == nullptr || receivedFrom == nullptr) {
		throw	std::bad_alloc();
	}

	sockaddr_in other{};
	int         length = sizeof(other);

	auto receivedLength = recvfrom(Socket,(char*)&message,bufferSize,0,(sockaddr*)&other,&length);
	if(receivedLength == SOCKET_ERROR && receivedLength > MAX_NETMESSAGE_SIZE) {
		return false;
	} else {
		receivedFrom->address = other.sin_addr.S_un.S_addr;
		receivedFrom->port = other.sin_port;
		return true;
	}
}

NetAddress::NetAddress() {
	if(inet_pton(AF_INET,LOCALHOST,&address) != 1) {
		throw std::exception("What the fuck");
	}
	port = htons(DEFAULT_PORT);
}

NetAddress::NetAddress(const std::string& Ip,unsigned short port) {
	if(inet_pton(AF_INET,Ip.c_str(),&this->address) != 1) {
		throw std::exception("Bad ip");
	}
	this->port = htons(port);
}

sockaddr NetAddress::asAddress() const {
	sockaddr_in intermediate{};
	std::memset(&intermediate,0,sizeof(intermediate));
	intermediate.sin_family = AF_INET;
	intermediate.sin_port = port;
	intermediate.sin_addr.S_un.S_addr = address;
	return *(sockaddr*)&intermediate;
}
