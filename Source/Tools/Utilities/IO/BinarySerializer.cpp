#include "BinarySerializer.h"

BinaryFileSerializer::BinaryFileSerializer(std::filesystem::path path) : path(path)
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

BinaryFileSerializer::~BinaryFileSerializer()
{
	stream.close();
}

void BinaryFileSerializer::Write(const char* data, size_t size)
{
	data; size;

	stream.write(data, size);
}

void BinaryFileDeserializer::Read(const char* data, size_t size)
{
	data; size;
}
