#include "PersistentSystems.pch.h"

#include "../NetworkStructs.h"
#include "../NetMessage/NetMessage.h"


NetworkConnection::Status NetworkConnection::StartAsServer(int socketType,int socketProtocol,unsigned short bindType) {


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
	server.sin_family = (ADDRESS_FAMILY)bindType;
	server.sin_port = homeAddress.port;
	server.sin_addr.S_un.S_addr = homeAddress.address;

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

NetworkConnection::Status NetworkConnection::AutoHostOrClient(int socketType,int socketProtocol,unsigned short bindType) {
	return StartAsClient(socketType,socketProtocol,bindType) == NetworkConnection::Status::initialized ? NetworkConnection::Status::initialized : StartAsServer(socketType,socketProtocol,bindType);
}

NetworkConnection::Status NetworkConnection::StartAsClient(int socketType,int socketProtocol,unsigned short bindType) {

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
	server.sin_port = homeAddress.port;
	server.sin_addr.S_un.S_addr = homeAddress.address;

	if(connect(Socket,(sockaddr*)&server,sizeof(server)) == SOCKET_ERROR) {
		std::cerr << "Could not connect to server: " << WSAGetLastError() << "\n";
		status = Status::failed;
		Socket = INVALID_SOCKET;
		return status;
	}

	status = Status::initialized;
	return status;
}

bool NetworkConnection::Send(const NetMessage& message,const NetAddress& sendTo) const {
	sockaddr addr = sendTo.asAddress();
	int length = sizeof(addr);

	if(sendto(Socket,(const char*)&message,sizeof(message),0,&addr,length) == SOCKET_ERROR) {
		return false; 
	}
	return true;
}

bool NetworkConnection::Close() const {
	return closesocket(Socket) == 0;
}

bool NetworkConnection::Receive(NetMessage* const message,NetAddress* recivedFrom,const int bufferSize,const float timeout) const {
	UNREFERENCED_PARAMETER(timeout);

	//static_assert(bufferSize <= 512);
	assert(bufferSize <= 512);

	if(status != Status::failed) {
		throw	std::overflow_error("To large buffer, max size is 512");
	}

	if(bufferSize > 512) {
		throw	std::overflow_error("To large buffer, max size is 512");
	}

	if(message->GetBuffer() == nullptr || recivedFrom == nullptr) {
		throw	std::bad_alloc();
	}

	sockaddr_in other{};
	int         length = sizeof(other);

	auto receivedLength = recvfrom(Socket,(char*)&message,bufferSize,0,(sockaddr*)&other,&length);
	if(receivedLength == SOCKET_ERROR && receivedLength > MAX_NETMESSAGE_SIZE) {
		return false;
	} else {
		recivedFrom->address = other.sin_addr.S_un.S_addr;
		recivedFrom->port = other.sin_port;
		return true;
	}
}

NetAddress::NetAddress() {
	if(inet_pton(AF_INET,LOCALHOST,&address) != 1) {
		throw std::exception("What the fuck");
	}
	port = htons(DEFAULT_PORT);
}

sockaddr NetAddress::asAddress() const {
	sockaddr_in intermediate{};
	std::memset(&intermediate,0,sizeof(intermediate));
	intermediate.sin_family = AF_INET;
	intermediate.sin_port = port;
	intermediate.sin_addr.S_un.S_addr = address;
	return *(sockaddr*)&intermediate;
}
