#pragma once
#include <Engine/AssetManager/ComponentSystem/Component.h>
#include <Engine/AssetManager/Objects/BaseAssets/GraphAsset.h>

class GraphAsset;

class GraphComponent : public Component
{
    friend class GraphAsset;

  public:
    MYLIB_REFLECTABLE();
    GraphComponent() = delete;
    explicit GraphComponent(const SY::UUID anOwnerID, GameObjectManager *aManager);
    explicit GraphComponent(const SY::UUID anOwnerID, GameObjectManager *aManager, const std::filesystem::path &path);

    void Update() override;
    bool InspectorView() override;

    void OnColliderEnter(const SY::UUID aGameObjectID) override;
    void OnColliderExit(const SY::UUID aGameObjectID) override;

  private:
    std::shared_ptr<GraphAsset> m_Graph;
};

REFL_AUTO(type(GraphComponent))
