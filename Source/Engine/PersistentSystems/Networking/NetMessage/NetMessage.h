#pragma once 
#include <string>
#include <array>
#include <chrono>
#include <Engine/PersistentSystems/Networking/NetworkStructs.h>

enum class eNetMessageType : unsigned char {
	None,
	ChatMessage,
	NewConnection,
	NicknameSet,
	Quit,
	PlayerJoin,
	TransformSyncMessage,
	CreateObjectMessage,
	DestroyObjectMessage,
	HearthBeat,
	Count
};

constexpr size_t MAX_NETMESSAGE_SIZE = 512;
constexpr size_t NETMESSAGE_HEADERSIZE = sizeof(eNetMessageType) + sizeof(NetworkedId) + sizeof(std::chrono::steady_clock::time_point);
constexpr size_t NETMESSAGE_BUFFERSIZE = (MAX_NETMESSAGE_SIZE - NETMESSAGE_HEADERSIZE);

#undef GetMessage
#undef GetMessageW


using TimePoint = std::chrono::high_resolution_clock::time_point;
using Duration = std::chrono::high_resolution_clock::duration;

//Recieve directly into NetMessage address please..

class NetMessage {
	friend struct NetworkConnection;
	friend class NetworkRunner;
public:
	NetMessage() = default;
	explicit NetMessage(NetworkedId id) : Id(id)  {}
	~NetMessage() = default;

	char* GetBuffer() {
		return dataBuffer.data();
	}
	NetworkedId GetId() const {
		return  Id;
	}
	TimePoint TimeSent() const {
		return  timePoint;
	}
	void SetId(NetworkedId id){
		Id = id;
	}
protected:
	NetworkedId Id;
	TimePoint timePoint;
	std::array<char,NETMESSAGE_BUFFERSIZE> dataBuffer{}; 
public:
	eNetMessageType myType = eNetMessageType::None;
	constexpr static eNetMessageType type = eNetMessageType::None;

};
