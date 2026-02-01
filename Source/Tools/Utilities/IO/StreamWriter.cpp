#include "StreamWriter.h"

FileWriter::FileWriter(std::filesystem::path path) : path(path)
{

	if (path.has_parent_path())
	{
		std::error_code code;
		bool fail = !std::filesystem::create_directories(path.parent_path(), code);
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

void FileReader::Read(const char* data, size_t size)
{
	data; size;
}
