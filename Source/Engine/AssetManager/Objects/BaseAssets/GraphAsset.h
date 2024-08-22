#pragma once
#include "BaseAsset.h"

class ScriptGraph;

class GraphAsset : public AssetBase
{
    friend class GraphComponent;

  public:
    MYLIB_REFLECTABLE();
    // Json path
    explicit GraphAsset(const std::filesystem::path &aFilePath); // Json path
    void Init() override;
    bool InspectorView() override;
    std::shared_ptr<TextureHolder> GetEditorIcon() override;

  private:
    std::shared_ptr<ScriptGraph> graphToEdit;
};
REFL_AUTO(type(GraphAsset))
