#include "pch.h"

#include "ScriptGraphTypes.h"

extern "C" void __MuninGraph_AutoRegTypes()
{
}

//~ Internal type to handle Owner variable on ScriptGraphs.
IMPLEMENT_REGISTERED_TYPE(void *, Internal_VoidPtr);
//~

IMPLEMENT_REGISTERED_TYPE(size_t, Size_t);
IMPLEMENT_REGISTERED_TYPE(bool, Bool);
IMPLEMENT_REGISTERED_TYPE(int, Int);
IMPLEMENT_REGISTERED_TYPE(float, Float);
IMPLEMENT_REGISTERED_TYPE(SY::UUID, tUUID);
IMPLEMENT_REGISTERED_TYPE(GameObject, tGameObject);
IMPLEMENT_REGISTERED_TYPE(Vector3f, tVector3f);

BEGIN_IMPLEMENT_REGISTERED_TYPE(std::string, String);
registeredType.myCopyFunc = [](const std::shared_ptr<void> &aOtherPtr) {
    if (const std::shared_ptr<const std::string> ptrT = std::static_pointer_cast<const std::string>(aOtherPtr))
    {
        return std::make_shared<std::string>(*ptrT);
    }
    return std::make_shared<std::string>();
};
registeredType.mySerializeFunc = []([[maybe_unused]] const RegisteredType &ThisType, const void *aDataPtr,
                                    std::vector<uint8_t> &outDataBlock) {
    // Strings are "fun" :P
    // Let's assume that this is a normal string...
    const std::string *strPtr = static_cast<const std::string *>(aDataPtr);

    // We can't trust the size in typeinfo for the size of a string data
    // block. Since we're not wstring we're just string length * sizeof(char)
    // which is string length in bytes.
    outDataBlock.resize(strPtr->length() * sizeof(char));
    memcpy_s(outDataBlock.data(), outDataBlock.size(), strPtr->data(), strPtr->length());
    return true;
};
registeredType.myDeserializeFunc = []([[maybe_unused]] const RegisteredType &ThisType,
                                      const std::vector<uint8_t> &aDataBlock, void *outDataPtr) {
    // And when we build a string back from a vector we need to make sure it's
    // sized properly. outDataPtr is already initialized as a pointer of the
    // correct type.
    std::string *strPtr = static_cast<std::string *>(outDataPtr);
    *strPtr = std::string(aDataBlock.begin(), aDataBlock.end());
    return true;
};
END_IMPLEMENT_REGISTERED_TYPE();
