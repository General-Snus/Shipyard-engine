#pragma once
#include <Engine/AssetManager/ComponentSystem/Component.h>
#include <Engine/AssetManager/Objects/BaseAssets/GraphAsset.h>
#include <filesystem>
#include <memory>

class GraphComponent : public Component
{
    friend class GraphAsset;

  public:
    MYLIB_REFLECTABLE();

  private:
    std::shared_ptr<GraphAsset> myGraph;
};

REFL_AUTO(type(GraphComponent))
