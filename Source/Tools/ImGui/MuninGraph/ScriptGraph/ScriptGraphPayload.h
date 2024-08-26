#pragma once
#include <format>
#include <string>
#include <unordered_map>

#include <Tools/ImGui/MuninGraph/Internal/MuninGraphCommon.h>
#include <Tools/ImGui/MuninGraph/Types/TypeRegistry.h>

class ScriptGraphPayload
{
    friend class ScriptGraphNode;

  public:
    template <typename T> void SetPinValue(const std::string &aString, const T &aValue)
    {
        const RegisteredType *registeredT = TypeRegistry::Get().Resolve(typeid(T));

        if (const auto it = myData.find(aString); it != myData.end())
        {
            TypedDataContainer &container = it->second;
            if (container.GetType() != registeredT)
            {
                const std::string message =
                    std::format("Attempted to set a Payload value of type {} with a value of type {}!",
                                container.GetType()->GetFriendlyName(), registeredT->GetFriendlyName());
                throw std::invalid_argument(message);
            }
            container.TrySet(aValue);
        }
        else
        {
            myData.emplace(aString, std::move(TypedDataContainer::Create(registeredT)));
            TypedDataContainer &container = myData.at(aString);
            container.TrySet(aValue);
        }
    }

    template <typename T> bool TryGetPinValue(const std::string &aString, T &outValue) const
    {
        const RegisteredType *registeredT = TypeRegistry::Get().Resolve(typeid(T));

        if (const auto it = myData.find(aString); it != myData.end())
        {
            const TypedDataContainer &container = it->second;
            if (container.GetType() == registeredT)
            {
                return container.TryGet(outValue);
            }
        }

        return false;
    }

  private:
    std::unordered_map<std::string, TypedDataContainer, string_hash, std::equal_to<>> myData;
};
