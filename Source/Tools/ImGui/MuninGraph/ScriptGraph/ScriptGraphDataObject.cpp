#include "MuninGraph.pch.h"
#include "ScriptGraphDataObject.h"

#include "ScriptGraphTypes.h"

void ScriptGraphDataObject::SetDataInternal(const void* aValue, size_t aSize)
{
	// FY!
	//memcpy_s(Ptr, TypeData->GetTypeSize(), &aValue, aSize);
	memcpy_s(Ptr, TypeData->GetTypeSize(), aValue, aSize);
}

void ScriptGraphDataObject::GetDataInternal(void* aValue, size_t aSize) const
{
	memcpy_s(aValue, aSize, Ptr, TypeData->GetTypeSize());
}

void ScriptGraphDataObject::CreateInternal(ScriptGraphDataObject& aDataObject, std::type_index aType)
{
	aDataObject.TypeData = ScriptGraphDataTypeRegistry::GetType(aType);
	aDataObject.Ptr = malloc(aDataObject.TypeData->GetTypeSize());
}

ScriptGraphDataObject::ScriptGraphDataObject(const std::type_index& aType): Ptr(nullptr)
{
	TypeData = ScriptGraphDataTypeRegistry::GetType(aType);
}

ScriptGraphDataObject::ScriptGraphDataObject(ScriptGraphDataObject&& other) noexcept
{
	std::swap(Ptr, other.Ptr);
	std::swap(TypeData, other.TypeData);
}

ScriptGraphDataObject::~ScriptGraphDataObject()
{
	TypeData->DestroyTypeObject(*this);
	//free(Ptr);
	TypeData = nullptr;
}

ScriptGraphDataObject& ScriptGraphDataObject::operator=(const ScriptGraphDataObject& other)
{
	if (this != &other)
	{
		if (Ptr)
		{
			TypeData->DestroyTypeObject(*this);
			//free(Ptr);
		}

		if (other.TypeData)
		{
			TypeData = ScriptGraphDataTypeRegistry::GetType(other.TypeData->GetType());
			if (other.Ptr)
			{
				const size_t ptrSize = TypeData->GetTypeSize();
				Ptr = new char[ptrSize];
				//Ptr = malloc(ptrSize);
				memset(Ptr, 0, ptrSize);
				memcpy_s(Ptr, ptrSize, other.Ptr, other.TypeData->GetTypeSize());
				if(TypeData->GetType() == typeid(std::string))
				{
					std::string* ptrOther = static_cast<std::string*>(other.Ptr);
					std::string* ptrThis = static_cast<std::string*>(Ptr);
					int a = 1;
				}
			}
		}
		else
		{
			TypeData = nullptr;
			Ptr = nullptr;
		}
	}

	return *this;
}
