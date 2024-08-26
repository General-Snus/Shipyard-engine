#pragma once
#include "Tools/ImGui/MuninGraph/NodeGraph/NodeGraphCommon.h"
#include "Tools/ImGui/MuninGraph/Types/TypeRegistry.h"

class ScriptGraphPin;

// Contains information on how to display a RegisteredType in the editor.
struct ScriptGraphEditorType
{
    FORCEINLINE const RegisteredType *GetType() const
    {
        return myType;
    }
    FORCEINLINE std::type_index GetTypeInfo() const
    {
        return myType->GetType();
    }

    ScriptGraphEditorType(const RegisteredType *aType);
    virtual ~ScriptGraphEditorType();

    virtual FORCEINLINE const GraphColor &GetTypeColor() const
    {
        return GraphColor::White;
    }
    virtual FORCEINLINE bool IsInlineEditable() const
    {
        return false;
    }

    virtual bool TypeEditWidget(std::string_view aUniqueName, const TypedDataContainer &aDataContainer) const;
    virtual std::string ToString(const TypedDataContainer &aDataContainer) const;
    virtual FORCEINLINE bool IsInternal() const
    {
        return false;
    }

  private:
    const RegisteredType *myType;
};

class ScriptGraphEditorTypeRegistry
{
  public:
    static ScriptGraphEditorTypeRegistry &Get()
    {
        static ScriptGraphEditorTypeRegistry thisInstance;
        return thisInstance;
    }

    bool Initialize()
    {
        if (myEditorTypes.empty())
        {
            for (const auto &typeReg : myStaticRegistrations)
            {
                auto type = typeReg();
                // const RegisteredType *regType = type->GetType();
                const auto typeRef = myEditorTypes.emplace(type->GetTypeInfo(), std::move(type));
                myEditorTypeNames.emplace_back(typeRef.first->second->GetType()->GetFriendlyName());
                myEditorTypeNameToTypeIndex.emplace(typeRef.first->second->GetType()->GetFriendlyName(),
                                                    typeRef.first->second->GetType()->GetType());
            }
        }

        return !myEditorTypes.empty();
    }

    const ScriptGraphEditorType *GetEditorType(const RegisteredType *aType) const
    {
        if (myEditorTypes.contains(aType->GetType()))
        {
            return myEditorTypes.at(aType->GetType()).get();
        }

        return nullptr;
    }

    const ScriptGraphEditorType *GetEditorType(std::string_view aTypeName) const
    {
        if (myEditorTypeNameToTypeIndex.contains(aTypeName))
        {
            const auto it = myEditorTypeNameToTypeIndex.find(aTypeName);
            return myEditorTypes.at(it->second).get();
        }

        return nullptr;
    }

    typedef std::function<std::unique_ptr<ScriptGraphEditorType>()> CreateEditorTypeFunc;

    void RegisterEditorType(const CreateEditorTypeFunc &aFunc)
    {
        myStaticRegistrations.push_back(aFunc);
    }

    FORCEINLINE const std::unordered_map<std::type_index, std::unique_ptr<ScriptGraphEditorType>> &GetEditorTypes()
        const
    {
        return myEditorTypes;
    }
    FORCEINLINE const std::vector<std::string_view> &GetEditorTypeNames() const
    {
        return myEditorTypeNames;
    }

  private:
    std::vector<CreateEditorTypeFunc> myStaticRegistrations;

    std::unordered_map<std::type_index, std::unique_ptr<ScriptGraphEditorType>> myEditorTypes;
    std::vector<std::string_view> myEditorTypeNames;
    std::unordered_map<std::string, std::type_index, string_hash, std::equal_to<>> myEditorTypeNameToTypeIndex;
};

template <class EditorTypeClass> struct AutoRegisterEditorType
{
    AutoRegisterEditorType()
    {
        ScriptGraphEditorTypeRegistry::Get().RegisterEditorType([this] { return GetEditorType(); });
    }

    std::unique_ptr<ScriptGraphEditorType> GetEditorType()
    {
        return std::make_unique<EditorTypeClass>();
    }
};

/**
 * \brief Declares a Type that the Script Graph Editor should be aware of.
 * \param T The C++ type that we are declaring. Must exist in the TypeRegistry!
 * \param N The friendly name of the type that will be used in the UI and Serialization.
 * \param C A GraphColor to be used in the UI when showing this type. Primarily on Node Pins.
 * \param E A Boolean describing if this type is Inline Editable or not (can be edited on Nodes / Variable Editor).
 *			If True you must also override and implement TypeEditWidget() to get this functionality!
 */
#define BEGIN_DECLARE_EDITOR_TYPE(T, N, C, E)                                                                          \
    struct ScriptGraphEditorType_##N : public ScriptGraphEditorType                                                    \
    {                                                                                                                  \
        FORCEINLINE const GraphColor &GetTypeColor() const override                                                    \
        {                                                                                                              \
            static GraphColor myColor = C;                                                                             \
            return myColor;                                                                                            \
        }                                                                                                              \
        FORCEINLINE bool IsInlineEditable() const override                                                             \
        {                                                                                                              \
            return E;                                                                                                  \
        }                                                                                                              \
        ScriptGraphEditorType_##N##()

#define END_DECLARE_EDITOR_TYPE() }

#define IMPLEMENT_EDITOR_TYPE(T, N)                                                                                    \
    static AutoRegisterEditorType<ScriptGraphEditorType_##N##> zz__##N##EditorTypeRegistration;                        \
    ScriptGraphEditorType_##N## ::ScriptGraphEditorType_##N##()                                                        \
        : ScriptGraphEditorType(TypeRegistry::Get().Resolve(typeid(T)))                                                \
    {                                                                                                                  \
    }
