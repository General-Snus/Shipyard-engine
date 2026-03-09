#pragma once
#include "../LinearAlgebra/Vectors.hpp"
#include "Serializer.h" 
#include <filesystem>
#include <fstream>
#include <string>



struct SerializableTag {};
class BinarySerializer : Serializer
{
public:
	bool IsBinary() const override { return true; };

	virtual void Write(const char* data, size_t size) = 0;

	template<typename T>
	void WriteBinary(const T data);
	void WriteText(const std::string text);
	void WritePath(const std::filesystem::path path);

	template<typename T>
	void WriteVector(const std::vector<T>& value);
	template<typename T>
	void WriteArray(const T* value, size_t size);
};

class BinaryFileSerializer : public BinarySerializer
{
public:
	BinaryFileSerializer(std::filesystem::path path);
	~BinaryFileSerializer();
	void Write(const char* data, size_t size) override;
private:
	std::filesystem::path path;
	std::ofstream stream;
};


class BinaryMemorySerializer : public BinarySerializer
{
};

class BinaryDeserializer : Deserializer
{
public:
	virtual void Read(const char* data, size_t size) = 0;
};

class BinaryFileDeserializer : public BinaryDeserializer
{
public:
	BinaryFileDeserializer(std::filesystem::path path);
	~BinaryFileDeserializer();
	void Read(const char* data, size_t size) override;
private:
	std::filesystem::path path;
	std::ofstream stream;
};

class BinaryMemoryDeserializer : public BinaryDeserializer
{
};

template<typename T>
inline void BinarySerializer::WriteBinary(const T data)
{
	Write(&data, sizeof(T));
}

inline void BinarySerializer::WriteText(const std::string text)
{
	Write(text.data(), text.size());
}

inline void BinarySerializer::WritePath(const std::filesystem::path path)
{
}

template<typename T>
inline void BinarySerializer::WriteArray(const T* value, size_t size)
{
}

template<typename T>
inline void BinarySerializer::WriteVector(const std::vector<T>& value)
{
}
