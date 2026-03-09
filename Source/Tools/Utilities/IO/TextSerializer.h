#pragma once
#include "Serializer.h"
#include <External\nlohmann\json.hpp>

class TextWriterBackend {

};

class MemoryBackend :TextWriterBackend {

};

class FileBackend : TextWriterBackend {

};

class TextSerializer : Serializer {
public:
	TextSerializer(TextWriterBackend* writerBackend) : writerImplementation(writerBackend) {}
	bool IsBinary() const override { return false; };
	virtual void BeginObject(const char* name) = 0;
	virtual void EndObject() = 0;
	virtual ~TextSerializer() = default;

	virtual void Write(const char* name, float& value) = 0;

	TextWriterBackend* writerImplementation;
};

class JsonArchive : public TextSerializer {
public:


private:
	nlohmann::json jsonOut;
};



class TextDeserializer : Deserializer {
public:
	TextDeserializer() {}
	bool IsBinary() const override { return false; };
	virtual void BeginObject(const char* name) = 0;
	virtual void EndObject() = 0;
	virtual ~TextDeserializer() = default;

	virtual void Write(const char* name, float& value) = 0;
};
