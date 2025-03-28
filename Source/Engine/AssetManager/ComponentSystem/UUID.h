#pragma once
#include <string>
#include <xhash>
namespace SY
{ // I fucking hate windows internals

class UUID
{
  public:
    UUID() = default;
    UUID(uint32_t aUUID) : myUUID(aUUID) {};
    UUID(const UUID &aUUID) = default;
    operator uint32_t() const
    {
        return myUUID;
    }
    operator std::string() const
    {
        return std::to_string(myUUID);
    }

    bool IsValid() const
    {
        return myUUID != UINT_MAX;
    }
    static UUID InvalidID()
    {
        return UINT_MAX;
    } 
  private:
    uint32_t myUUID;
};

inline bool operator==(UUID anUUID0, UUID anUUID1)
{
    return (static_cast<uint32_t>(anUUID0) == static_cast<uint32_t>(anUUID1));
}

inline bool operator==(UUID anUUID0, int anID)
{
    return (static_cast<uint32_t>(anUUID0) == static_cast<uint32_t>(anID));
}

inline bool operator!=(UUID anUUID0, UUID anUUID1)
{
    return (static_cast<uint32_t>(anUUID0) != static_cast<uint32_t>(anUUID1));
}

inline bool operator!=(UUID anUUID0, int anID)
{
    return (static_cast<uint32_t>(anUUID0) != static_cast<uint32_t>(anID));
}
} // namespace SY

template <typename T> struct hash;
template <> struct std::hash<SY::UUID>
{
    size_t operator()(const SY::UUID &uuid) const
    {
        return hash<uint64_t>()(static_cast<uint32_t>(uuid));
    }
};
