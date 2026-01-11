#include "StreamWriter.h"

FileWriter::FileWriter(std::filesystem::path path) : path(path)
{

	if (!std::filesystem::exists(path))
	{
		std::error_code code;
		bool fail = std::filesystem::create_directories(path, code);
		if (fail)
		{
			//__debugbreak();
		}
	}
	stream = std::ofstream(path, std::ofstream::out | std::ofstream::binary);
}

FileWriter::~FileWriter()
{
	stream.close();
}

void FileWriter::Write(const char* data, size_t size)
{
	data; size;

	stream.write(data, size);
}
