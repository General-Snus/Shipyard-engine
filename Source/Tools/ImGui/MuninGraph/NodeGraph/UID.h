#pragma once
#include <cassert>
#include <regex>
#include <string_view>
#include <typeindex>
#include <typeinfo>
#include <utility>
/// <summary>
/// Represents a execution unique ID number.
/// </summary>
class UID
{
  public:
    // ctors get a new UID
    UID() : myUID(nextUID++)
    {
    }

    // copies get their own UID
    UID(const UID &) : myUID(nextUID++)
    {
    }

    // Moved objects retain their UID
    UID(UID &&other) noexcept : myUID(other.myUID)
    {
    }

    UID &operator=(const UID &aOther)
    {
        myUID = aOther.myUID;
        return *this;
    }

    UID &operator=(UID &&aOther) noexcept
    {
        std::swap(myUID, aOther.myUID);
        return *this;
    }

    __forceinline size_t GetUID() const
    {
        return myUID;
    }
    __forceinline operator const size_t() const
    {
        return myUID;
    }

  private:
    static size_t nextUID;
    size_t myUID = 0;
};

// Allow UID as key in a map.
template <> struct std::hash<UID>
{
    auto operator()(const UID &aUID) const noexcept -> size_t
    {
        return std::hash<size_t>{}(aUID.GetUID());
    }
};

/// <summary>
/// Represents a named UID. Has a execution unique ID number
/// and a type-unique name and instance counter.
/// </summary>
class ObjectUID : public UID
{
  public:
    __forceinline std::string_view Get() const
    {
        return myUniqueName;
    }
    virtual __forceinline std::string_view GetTypeName() const = 0;
    virtual __forceinline std::type_index GetType() const = 0;
    __forceinline std::string_view GetUniqueName() const
    {
        return myUniqueName;
    }

  protected:
    ObjectUID() = default;
    virtual ~ObjectUID() = default;
    ObjectUID(ObjectUID &&other) noexcept
        : UID(std::move(other)), myUniqueName(std::move(other.myUniqueName)), myInstance(other.myInstance)
    {
    }

    std::string myUniqueName = "Invalid Name";
    size_t myInstance = 0;
};

/// <summary>
/// CRTP base for objects wishing to have ObjectUIDs.
/// </summary>
/// <typeparam name="T">The type inheriting from TypeObjectUID.</typeparam>
template <typename T> class TypeObjectUID : public ObjectUID
{
  public:
    TypeObjectUID()
    {
        myInstance = nextInstance++;
        myUniqueName = GetTypeName();
        myUniqueName = "##_" + myUniqueName + "_" + std::to_string(myInstance);
    }

    TypeObjectUID(TypeObjectUID &&other) noexcept : ObjectUID(std::move(other))
    {
    }

    ~TypeObjectUID() override = default;

    __forceinline std::string_view GetTypeName() const override
    {
        return myTypeName;
    }
    __forceinline std::type_index GetType() const override
    {
        return myType;
    }

  private:
    static std::string DeriveMSVCTypeName()
    {
        const std::string msvcTypeName = myType.name();
        const size_t fromPos = msvcTypeName.find_first_of(' ');
        size_t toPos = msvcTypeName.find_first_of(' ', fromPos);
        if (toPos == std::string::npos)
        {
            toPos = msvcTypeName.size() - fromPos;
        }
        else
        {
            toPos = msvcTypeName.size() - toPos;
        }

        std::string result = msvcTypeName.substr(fromPos + 1, toPos);
        try
        {
            result =
                std::regex_replace(result, std::regex(R"((?:\bclass\b|\bstruct\b)\s+([a-zA-Z0-9_-]+)(?:[\s:<]*))"), "");
        }
        catch (std::exception &e)
        {
            std::string m = e.what();
            assert(false); // Need more testing here.
        }

        return result;
    }

    static std::type_index myType;
    static inline size_t nextInstance = 0;
    static std::string myTypeName;
};

template <typename T> std::type_index TypeObjectUID<T>::myType = typeid(T);

template <typename T> std::string TypeObjectUID<T>::myTypeName = DeriveMSVCTypeName();

#define AsObjectUIDPtr(Ptr) dynamic_cast<const ObjectUID *>(Ptr)
#define AsObjectUIDSharedPtr(SharedPtr) std::dynamic_pointer_cast<const ObjectUID>(SharedPtr)
