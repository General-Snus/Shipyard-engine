#pragma once
#include <filesystem>
#include <fstream>


struct SerializableTag {};



class StreamWriter
{
public:
	virtual void Write(const char* data, size_t size) = 0;
};

class FileWriter : public StreamWriter
{
public:
	FileWriter(std::filesystem::path path);
	~FileWriter();
	void Write(const char* data, size_t size) override;
private:
	std::filesystem::path path;
	std::ofstream stream;
};

class MemoryWriter : public StreamWriter
{
};

class StreamReader
{
public:
	virtual void Read(const char* data, size_t size) = 0;
};

class FileReader : public StreamReader
{
public:
	FileReader(std::filesystem::path path);
	~FileReader();
	void Read(const char* data, size_t size) override;
private:
	std::filesystem::path path;
	std::ofstream stream;
};

class MemoryReader : public StreamReader
{
};
