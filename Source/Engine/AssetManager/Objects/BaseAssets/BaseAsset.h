#pragma once 
#include "Engine/AssetManager/Reflection/Reflectable.h" 
#include "Engine/AssetManager/Interfaces.h" 
#include <filesystem>
#include <functional>
#include <memory>

class Scene;
class TextureHolder;
 
class AssetBase : public AvailableInInspector, public Reflectable<AssetBase>
{
public:
	reflectable(AssetBase)
	AssetBase(const std::filesystem::path& aFilePath);
	virtual ~AssetBase() = default;
	// When overriding you have the responisiblitity to set the isloadedcomplete flag
	virtual void Init() = 0;

	const std::filesystem::path& GetAssetPath() const;
	bool InspectorView() override;
	virtual std::shared_ptr<TextureHolder> GetEditorIcon();

	virtual bool InjectIntoScene(std::shared_ptr<Scene> SceneToAddAsset);

	//Warning,this is a blocking operation, timeout is seconds, negative is infinite block
	void WaitForReady(float timeout = 100.0f) const;

	std::filesystem::path AssetPath;
	bool isLoadedComplete = false;
	bool isBeingLoaded = false;
	std::vector<std::function<void()>> callBackOnFinished;
};

REFL_AUTO(type(AssetBase), field(AssetPath), field(isLoadedComplete), field(isBeingLoaded))
