#pragma once
/*
 * Registered Type System
 * ======================
 * Allows easy type management for various things without knowing the specifics of each type
 * including serialization and deserialization to a byte array. Useful for various things such
 * as Pins and Variables in MuninGraph but also i.e. Blackboards or any other place where various
 * types need to be managed in a centralized manner.
 *
 * Usage:
 * in a Header file Declare one or more types:
 *
 * DECLARE_REGISTERED_TYPE(float);
 * DECLARE_REGISTERED_TYPE(std::string);
 *
 * in a Source file, Implement these types:
 * IMPLEMENT_REGISTERED_TYPE(double, Double);
 *
 * For non POD types like std::string you must provide your own copy, serialize and deserialize logic:
 *
 * BEGIN_IMPLEMENT_REGISTERED_TYPE(std::string, String);
 * registeredType.myCopyFunc = [](const std::shared_ptr<void>& aOtherPtr)
 * 	{
 * 		if (const std::shared_ptr<const std::string> ptrT = std::static_pointer_cast<const std::string>(aOtherPtr))
 * 		{
 * 			return std::make_shared<std::string>(*ptrT);
 * 		}
 * 		return std::make_shared<std::string>();
 * 	};
 * registeredType.mySerializeFunc = [](const RegisteredType& ThisType, const void* aDataPtr, std::vector<uint8_t>&
 * outDataBlock)
 * 	{
 * 		(ThisType);
 * 		// Strings are "fun" :P
 * 		// Let's assume that this is a normal string...
 * 		const std::string* strPtr = static_cast<const std::string*>(aDataPtr);
 *
 * 		// We can't trust the size in typeinfo for the size of a string data block.
 * 		// Since we're not wstring we're just string length * sizeof(char) which is string length in bytes.
 * 		outDataBlock.resize(strPtr->length() * sizeof(char));
 * 		memcpy_s(outDataBlock.data(), outDataBlock.size(), strPtr->c_str(), strPtr->length());
 * 		return true;
 * 	};
 * registeredType.myDeserializeFunc = [](const RegisteredType& ThisType, const std::vector<uint8_t>& aDataBlock, void*
 * outDataPtr)
 * 	{
 * 		// And when we build a string back from a vector we need to make sure it's sized properly.
 * 		// outDataPtr is already initialized as a pointer of the correct type.
 * 		(ThisType);
 * 		std::string* strPtr = static_cast<std::string*>(outDataPtr);
 * 		*strPtr = std::string(aDataBlock.begin(), aDataBlock.end());
 * 		return true;
 * 	};
 * END_IMPLEMENT_REGISTERED_TYPE();
 *
 * After this you can use these types with TypeDataContainer and TDataContainer:
 * TypeDataContainer container = TypedDataContainer::Create<float>();
 *
 * TDataContainer<std::string> myContainer;
 *
 * TDataContainer works as a wrapper around TypedDataContainer to allow easy interfacing
 * when the type is known. They are the same size as a TypedDataContainer and can be casted:
 *
 * TDataContainer<float>* floatContainer = container;
 * if(floatContainer) // use object.
 *
 * Invalid casts will return a nullptr.
 */

#include <algorithm>
#include <cassert>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <typeindex>

#ifndef FORCEINLINE
#define FORCEINLINE __forceinline
#endif
#include <iostream>

/// <summary>
/// Represents information about a specific Type including information on how to make and copy said type
/// and how to Serialize/Deserialize the type to bytes.
/// In this project it's primarily used to manage type information for Munin Graph but it can be used
/// by anything needing type information.
/// </summary>
class RegisteredType
{
  public:
    template <typename T> static RegisteredType Create()
    {
        RegisteredType obj;
        assert(false && "That type has not been declared with REGISTER_SIMPLE_TYPE!");
        return obj;
    }

    bool Serialize(const void *aDataPtr, std::vector<uint8_t> &outDataBlock) const
    {
        return mySerializeFunc(*this, aDataPtr, outDataBlock);
    }
    bool Deserialize(const std::vector<uint8_t> &aDataBlock, void *inoutDataPtr) const
    {
        return myDeserializeFunc(*this, aDataBlock, inoutDataPtr);
    }

    std::shared_ptr<void> MakePtr() const
    {
        return myMakeFunc();
    }
    std::shared_ptr<void> CopyPtr(const std::shared_ptr<void> &aPtr) const
    {
        return myCopyFunc(aPtr);
    }

    FORCEINLINE size_t GetSize() const
    {
        return mySize;
    }
    FORCEINLINE std::type_index GetType() const
    {
        return myType;
    }
    FORCEINLINE std::string_view GetFriendlyName() const
    {
        return myFriendlyName;
    }
    FORCEINLINE bool IsValid() const
    {
        return myType != typeid(std::nullptr_t);
    }

    FORCEINLINE bool operator==(const RegisteredType &aOther) const
    {
        return myType == aOther.myType;
    }

    FORCEINLINE bool operator!=(const RegisteredType &aOther) const
    {
        return !(*this == aOther);
    }

    const static RegisteredType InvalidType;

  private:
    std::type_index myType = typeid(std::nullptr_t);
    size_t mySize = 0;
    char myFriendlyName[64]{};

    typedef std::function<std::shared_ptr<void>()> MakePtrFunc;
    typedef std::function<std::shared_ptr<void>(const std::shared_ptr<void> &)> CopyPtrFunc;
    typedef std::function<bool(const RegisteredType &, const void *, std::vector<uint8_t> &)> SerializeTypeFunc;
    typedef std::function<bool(const RegisteredType &, const std::vector<uint8_t> &, void *)> DeserializeTypeFunc;

    MakePtrFunc myMakeFunc;
    CopyPtrFunc myCopyFunc;
    SerializeTypeFunc mySerializeFunc;
    DeserializeTypeFunc myDeserializeFunc;
};

/// <summary>
/// The registry keeping track of all RegisteredTypes.
/// Need to fetch the singleton and call Initialize() somewhere before attempting to use it!
/// </summary>
class TypeRegistry
{
  public:
    typedef std::function<RegisteredType()> CreateDeclaredTypeFunc;

    static TypeRegistry &Get()
    {
        static TypeRegistry thisRegistry;
        return thisRegistry;
    }

    void RegisterStaticType(const CreateDeclaredTypeFunc &aFunc)
    {
        myStaticTypeDeclarations.emplace_back(aFunc);
    }

    bool Initialize()
    {
        if (myDeclaredTypes.empty())
        {
            for (const auto &typeDecl : myStaticTypeDeclarations)
            {
                RegisteredType type = typeDecl();
                std::string typeName(type.GetFriendlyName());
                std::ranges::transform(typeName, typeName.begin(),
                                       [](unsigned char c) { return (unsigned char)std::tolower(c); });
                myFriendlyNameToType.emplace(typeName, type.GetType());
                myDeclaredTypes.emplace(type.GetType(), std::move(type));
            }
        }

        return !myDeclaredTypes.empty();
    }

    template <typename T> const RegisteredType *Resolve() const
    {
        return Resolve(typeid(T));
    }

    const RegisteredType *Resolve(std::type_index aType) const
    {
        const auto it = myDeclaredTypes.find(aType);
        if (it == myDeclaredTypes.end())
        {
            return nullptr;
        }
        return &it->second;
    }

    const RegisteredType *Resolve(const std::string &aFriendlyName) const
    {
        std::string lowerCase = aFriendlyName;
        std::ranges::transform(lowerCase, lowerCase.begin(),
                               [](unsigned char c) { return (unsigned char)std::tolower(c); });
        if (const auto it = myFriendlyNameToType.find(lowerCase); it != myFriendlyNameToType.end())
        {
            return &myDeclaredTypes.find(it->second)->second;
        }

        return nullptr;
    }

    const RegisteredType *Resolve(std::string_view aFriendlyName) const
    {
        const std::string lowerCase(aFriendlyName.begin(), aFriendlyName.end());
        return Resolve(lowerCase);
    }

    TypeRegistry(const TypeRegistry &) = delete;
    TypeRegistry(TypeRegistry &&) noexcept = delete;
    TypeRegistry &operator=(const TypeRegistry &) = delete;
    TypeRegistry &operator=(TypeRegistry &&) noexcept = delete;

  private:
    std::vector<CreateDeclaredTypeFunc> myStaticTypeDeclarations;

    std::unordered_map<std::type_index, RegisteredType> myDeclaredTypes;
    std::unordered_map<std::string, std::type_index> myFriendlyNameToType;

    TypeRegistry() = default;
    ~TypeRegistry() = default;
};

template <typename T> class TDataContainer;

/// <summary>
/// A container for typed data using the RegisteredType system.
/// Allows easy interaction with data without knowing the exact type.
/// </summary>
class TypedDataContainer
{
  public:
    template <typename T> static TypedDataContainer Create()
    {
        return Create(typeid(T));
    }

    static TypedDataContainer Create(const RegisteredType *aType)
    {
        TypedDataContainer obj;
        obj.myType = aType;
        if (obj.myType == nullptr)
        {
            throw std::invalid_argument("That type has not been registered!");
        }
        obj.myData = obj.myType->MakePtr();

        return obj;
    }

    static TypedDataContainer Create(std::type_index aType)
    {
        return Create(TypeRegistry::Get().Resolve(aType));
    }

    TypedDataContainer() = default;
    virtual ~TypedDataContainer()
    {
        myType = nullptr;
        myData.reset();
        myData = nullptr;
    }

    TypedDataContainer(const TypedDataContainer &aOther)
    {
        if (this != &aOther)
        {
            if (myData)
            {
                myData.reset();
                myData = nullptr;
            }

            myType = aOther.myType;
            myData = myType->CopyPtr(aOther.myData);
        }
    }
    TypedDataContainer(TypedDataContainer &&aOther) noexcept
    {
        std::swap(myType, aOther.myType);
        std::swap(myData, aOther.myData);
    }

    TypedDataContainer &operator=(const TypedDataContainer &aOther)
    {
        if (this == &aOther)
            return *this;

        if (myData)
        {
            myData.reset();
            myData = nullptr;
        }

        myType = aOther.myType;
        myData = myType->CopyPtr(aOther.myData);

        return *this;
    }
    TypedDataContainer &operator=(TypedDataContainer &&aOther) noexcept
    {
        std::swap(myType, aOther.myType);
        std::swap(myData, aOther.myData);

        return *this;
    }

    FORCEINLINE const RegisteredType *GetType() const
    {
        return myType;
    }

    bool operator==(const TypedDataContainer &other) const
    {
        return myType == other.myType && myData == other.myData;
    }
    bool operator!=(const TypedDataContainer &other) const
    {
        return !(*this == other);
    }

    template <typename T> bool TryGet(T &outValue) const
    {
        if (typeid(T) == myType->GetType())
        {
            std::shared_ptr<T> ptrT = std::static_pointer_cast<T>(myData);
            outValue = *ptrT;
            return true;
        }

        return false;
    }

    template <typename T> bool TrySet(const T &aValue) const
    {
        if (typeid(T) == myType->GetType())
        {
            std::shared_ptr<T> ptrT = std::static_pointer_cast<T>(myData);
            *(ptrT) = aValue;
            return true;
        }

        return false;
    }

    void Serialize(std::vector<uint8_t> &outSerializedData) const
    {
        assert(IsValid());
        myType->Serialize(myData.get(), outSerializedData);
    }

    void Deserialize(const std::vector<uint8_t> &aDataBlock) const
    {
        assert(IsValid());
        myType->Deserialize(aDataBlock, myData.get());
    }

    template <typename T> operator TDataContainer<T> *()
    {
        if (typeid(T) == myType->GetType())
        {
            return reinterpret_cast<TDataContainer<T> *>(this);
        }

        return nullptr;
    }

    template <typename T> operator std::shared_ptr<T>()
    {
        return std::static_pointer_cast<T>(myData);
    }

    void *operator*() const
    {
        return myData.get();
    }

    FORCEINLINE bool IsValid() const
    {
        return *myType != RegisteredType::InvalidType && myData;
    }

  protected:
    const RegisteredType *myType = &RegisteredType::InvalidType;
    std::shared_ptr<void> myData; // This keeps track of the type deleter from make_shared.
};

/// <summary>
/// Hacker wrap around TypedDataContainer to allow creation of member variables
/// of a specific type. The TypeRegistry must be initialized before trying to use
/// these or they will not get created properly. Have the same size as TypedDataContainer
/// so can be used as an interface to them if the type is known by the coder.
/// </summary>
/// <typeparam name="T"></typeparam>
template <typename T> class TDataContainer : public TypedDataContainer
{
  public:
    TDataContainer()
    {
        myType = TypeRegistry::Get().Resolve<T>();
        myData = myType->MakePtr();
    }

    T Get() const
    {
        std::shared_ptr<T> ptrT = std::static_pointer_cast<T>(myData);
        return *ptrT;
    }

    void Set(const T &aValue) const
    {
        std::shared_ptr<T> ptrT = std::static_pointer_cast<T>(myData);
        *(ptrT) = aValue;
    }

    TDataContainer &operator=(const T &aValue)
    {
        Set(aValue);
        return *this;
    }

    operator T() const
    {
        std::shared_ptr<T> ptrT = std::static_pointer_cast<T>(myData);
        return *ptrT;
    }
};

/// <summary>
/// Auto registration of RegisteredTypes
/// </summary>
template <class TypeClass> struct AutoRegisterType
{
    AutoRegisterType()
    {
        TypeRegistry::Get().RegisterStaticType([this] { return CreateRegisteredType(); });
    }

    RegisteredType CreateRegisteredType()
    {
        return RegisteredType::Create<TypeClass>();
    }
};

// Declares the specific type as a Registered Type. This goes in the Header file.
#define DECLARE_REGISTERED_TYPE(T) template <> RegisteredType RegisteredType::Create<T>();

#define BEGIN_IMPLEMENT_REGISTERED_TYPE(T, N)                                                                          \
    static AutoRegisterType<T> zz__##N##TypeRegistration;                                                              \
    template <> RegisteredType RegisteredType::Create<T>()                                                             \
    {                                                                                                                  \
        static_assert(std::is_default_constructible_v<T>, "Registered Types must be Default Constructible!");          \
        RegisteredType registeredType;                                                                                 \
        std::string strName = #N;                                                                                      \
        memcpy_s(registeredType.myFriendlyName, std::size(registeredType.myFriendlyName), strName.data(),              \
                 strName.length());                                                                                    \
        registeredType.myType = typeid(T);                                                                             \
        registeredType.mySize = sizeof(T);                                                                             \
        registeredType.myMakeFunc = [] { return std::make_shared<T>(); }

#define END_IMPLEMENT_REGISTERED_TYPE()                                                                                \
    return registeredType;                                                                                             \
    }

// Implements the specific type as a Registered Type. This goes in the Source file.
// T = Type
// N = Friendly Name of the Type (I.e. String for std::string).
#define IMPLEMENT_REGISTERED_TYPE(T, N)                                                                                \
    BEGIN_IMPLEMENT_REGISTERED_TYPE(T, N);                                                                             \
    registeredType.myCopyFunc = [](const std::shared_ptr<void> &aOtherPtr) {                                           \
        auto copyPtr = std::static_pointer_cast<T>(aOtherPtr);                                                         \
        return std::make_shared<T>(*copyPtr);                                                                          \
    };                                                                                                                 \
    registeredType.mySerializeFunc = [](const RegisteredType &ThisType, const void *aDataPtr,                          \
                                        std::vector<uint8_t> &outDataBlock) {                                          \
        outDataBlock.resize(ThisType.GetSize());                                                                       \
        memcpy_s(outDataBlock.data(), ThisType.GetSize(), aDataPtr, ThisType.GetSize());                               \
        return true;                                                                                                   \
    };                                                                                                                 \
    registeredType.myDeserializeFunc = [](const RegisteredType &ThisType, const std::vector<uint8_t> &aDataBlock,      \
                                          void *outDataPtr) {                                                          \
        if (outDataPtr)                                                                                                \
        {                                                                                                              \
            memcpy_s(outDataPtr, ThisType.GetSize(), aDataBlock.data(), aDataBlock.size());                            \
            return true;                                                                                               \
        }                                                                                                              \
        return false;                                                                                                  \
    };                                                                                                                 \
    END_IMPLEMENT_REGISTERED_TYPE()
