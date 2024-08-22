#pragma once
#include <functional>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <utility>

#ifndef FORCEINLINE
#define FORCEINLINE __forceinline
#endif

#include "ScriptGraphDataObject.h"

enum class ScriptGraphNodeType : uint8_t
{
    Undefined,
    Function,
    Event,
};

class ScriptGraphType;

class ScriptGraphDataTypeRegistry
{
    typedef std::unordered_map<std::type_index, std::shared_ptr<ScriptGraphType>> TypeMap;

    static TypeMap &MyTypesMap()
    {
        static TypeMap myMap;
        return myMap;
    }

    static inline std::unordered_map<std::string, std::type_index> myStringToType;
    static inline std::unordered_map<std::string, std::type_index> myFriendlyNameToType;

    // TODO: I need a better idea for this one :P
    static inline bool typeStringsSorted = false;
    static inline std::vector<std::string> myTypeStrings;

  public:
    template <typename H, typename T> static bool Register()
    {
        assert(MyTypesMap().find(typeid(T)) == MyTypesMap().end() &&
               "A handler for that type has already been registered!");
        std::shared_ptr<H> ptr = std::make_shared<H>();
        ptr->MakeDataObject = [](ScriptGraphDataObject &aObject) { ScriptGraphDataObject::Create<T>(aObject); };
        ptr->DestroyDataObject = [](ScriptGraphDataObject &Obj) { ScriptGraphDataObject::Destruct<T>(Obj); };
        MyTypesMap().insert({typeid(T), ptr});
        myStringToType.insert({ptr->GetTypeName(), typeid(T)});
        myFriendlyNameToType.insert({ptr->GetFriendlyName(), typeid(T)});
        myTypeStrings.push_back(ptr->GetFriendlyName());
        return true;
    }

    static std::shared_ptr<const ScriptGraphType> GetType(const std::type_index &aType);
    static std::shared_ptr<const ScriptGraphType> GetType(const std::string &aType);
    static std::shared_ptr<const ScriptGraphType> GetTypeFromFriendlyName(const std::string &aFriendlyName);

    static void RenderEditInPlaceWidget(const std::type_index &aType, const std::string &aContainerUUID,
                                        void *aDataPtr);
    static std::string GetString(const std::type_index &aType, void *aDataPtr);
    static void Serialize(const std::type_index &aType, const void *aDataPtr, std::vector<uint8_t> &outData);
    static void Deserialize(const std::type_index &aType, void *outDataPtr, const std::vector<uint8_t> &inData);

    static std::string GetFriendlyName(const std::type_index &aType);
    static std::type_index GetTypeFromString(const std::string &aType);

    static ScriptGraphDataObject GetDataObjectOfType(const std::type_index &aType);
    static ScriptGraphDataObject GetDataObjectOfType(const std::string &aType);
    static GraphColor GetDataTypeColor(const std::type_index aType);
    static GraphColor GetDataTypeColorNormalized(const std::type_index aType);
    static bool IsTypeInPlaceConstructible(const std::type_index &aType);

    FORCEINLINE static const std::vector<std::string> &GetRegisteredTypeNames()
    {
        if (!typeStringsSorted)
        {
            std::sort(myTypeStrings.begin(), myTypeStrings.end());
            typeStringsSorted = true;
        }
        return myTypeStrings;
    }
};

class ScriptGraphType
{
    friend class ScriptGraphDataTypeRegistry;

    std::type_index Type = typeid(std::nullptr_t);
    std::string TypeName = "std::nullptr_t";
    std::string SimpleTypeName = "nullptr";
    std::string FriendlyName = "Nullptr";
    size_t TypeSize = 0;

    GraphColor Color = GraphColor::White;
    bool CanConstructInPlace = false;

    // Determines the actual type name for reflection purposes.
    [[nodiscard]] std::string FetchTypeName(const std::type_index &aType) const;
    // Determines the simple type name (i.e. for TemplateClass<A, B, C, D> it returns TemplateClass).
    [[nodiscard]] std::string FetchSimpleTypeName(const std::string &aFullName) const;

    ScriptGraphType() = default;

  public:
    typedef std::function<void(ScriptGraphDataObject &)> MakeDataObjectFunc;
    typedef std::function<void(ScriptGraphDataObject &)> DestroyDataObjectFunc;
    const static ScriptGraphType NullType;

  private:
    MakeDataObjectFunc MakeDataObject = nullptr;
    DestroyDataObjectFunc DestroyDataObject = nullptr;

  public:
    ScriptGraphType(const std::type_index &aType, size_t aTypeSize, std::string aFriendlyName, const GraphColor &aColor,
                    bool canConstructInPlace = false)
        : Type(aType), TypeName(FetchTypeName(Type)), SimpleTypeName(FetchSimpleTypeName(TypeName)),
          FriendlyName(std::move(aFriendlyName)), TypeSize(aTypeSize), Color(aColor),
          CanConstructInPlace(canConstructInPlace)
    {
    }

    virtual ~ScriptGraphType() = default;

    virtual void RenderConstructWidget(const std::string &aContainerUUID, void *aDataPtr,
                                       const ScriptGraphType &aTypeInfo)
    {
        aContainerUUID;
        aDataPtr;
        aTypeInfo;
    }
    virtual std::string ToString(const void *aDataPtr, const ScriptGraphType &aTypeInfo) const
    {
        aDataPtr;
        aTypeInfo;
        return "";
    }
    virtual void SerializeData(const void *aDataPtr, const ScriptGraphType &aTypeInfo,
                               std::vector<uint8_t> &outData) const;
    virtual void DeserializeData(const std::vector<uint8_t> &inData, const ScriptGraphType &aTypeInfo,
                                 void *outDataPtr) const;

    FORCEINLINE const std::type_index &GetType() const
    {
        return Type;
    }
    FORCEINLINE const std::string &GetTypeName() const
    {
        return TypeName;
    }
    FORCEINLINE const std::string &GetSimpleTypeName() const
    {
        return SimpleTypeName;
    }
    FORCEINLINE size_t GetTypeSize() const
    {
        return TypeSize;
    }
    FORCEINLINE const std::string &GetFriendlyName() const
    {
        return FriendlyName;
    }

    bool operator==(const ScriptGraphType &other) const
    {
        return Type == other.Type;
    }

    bool operator!=(const ScriptGraphType &other) const
    {
        return !(Type == other.Type);
    }

    operator const std::type_index &() const
    {
        return Type;
    }

    void DestroyTypeObject(ScriptGraphDataObject &aObject) const;
};

template <typename D, typename T> struct ScriptGraphTypeHandler
{
    static inline bool IsRegistered = ScriptGraphDataTypeRegistry::Register<D, T>();
};

#define BeginDataTypeHandler(N, T, C, B)                                                                               \
    struct N##ScriptGraphType : public ScriptGraphType, public ScriptGraphTypeHandler<N##ScriptGraphType, T>           \
    {                                                                                                                  \
        N##ScriptGraphType() : ScriptGraphType(typeid(T), sizeof(T), #N, C, B)                                         \
        {                                                                                                              \
        }
#define EndDataTypeHandler                                                                                             \
    }                                                                                                                  \
    ;
