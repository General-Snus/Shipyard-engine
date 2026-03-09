#pragma once
class Serializer
{
public:
	virtual ~Serializer() = default;

	virtual bool IsBinary() const = 0;
	virtual bool IsText() const { return !IsBinary(); }
};

class Deserializer
{
public:
	virtual ~Deserializer() = default;

	virtual bool IsBinary() const = 0;
	virtual bool IsText() const { return !IsBinary(); }
};

