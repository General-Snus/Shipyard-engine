#pragma once
#include <functional>
#include <Engine/AssetManager/Enums.h>
#include "DirectX/DX12/Graphics/GpuResource.h"
#include "Engine/AssetManager/Reflection/ReflectionTemplate.h"
#include "Engine/GraphicsEngine/Rendering/Vertex.h"
#include "Tools/Logging/Logging.h"
#include "Tools/Reflection/refl.hpp"

class Scene;
class TextureHolder;

class AssetBase : public Reflectable
{
public:
	ReflectableTypeRegistration();
	AssetBase(const std::filesystem::path& aFilePath);
	virtual ~AssetBase() = default;
	// When overriding you have the responisiblitity to set the isloadedcomplete flag
	virtual void Init() = 0;

	const std::filesystem::path& GetAssetPath() const
	{
		return AssetPath;
	};
	bool                                   InspectorView() override;
	virtual std::shared_ptr<TextureHolder> GetEditorIcon();

	virtual bool InjectIntoScene(std::shared_ptr<Scene> SceneToAddAsset)
	{
		LOGGER.Warn(
			std::format("Asset could not be added to scene\nAsset of type {} is not possible to load into scene",
			            GetTypeInfo().Name()));
		return false;
	};


	//Warning,this is a blocking operation, timeout is seconds, negative is infinite block
	void WaitForReady(float timeout = 100.0f)
	{
		assert(isBeingLoaded &&
			"You really should already be trying to load the bloody asset if you wait to wait for it to be ready");

		using fsec = std::chrono::duration<float>;
		if (timeout > 0.0f)
		{
			const auto startTime = std::chrono::system_clock::now();
			float      countdown = 0.0f;
			while (countdown > timeout && isLoadedComplete == false)
			{
				auto diff = std::chrono::system_clock::now() - startTime;
				countdown = std::chrono::duration_cast<fsec>(diff).count();
			}
			return;
		}

		while (isLoadedComplete == false)
		{
		}
	}

	std::filesystem::path              AssetPath;
	bool                               isLoadedComplete = false;
	bool                               isBeingLoaded = false;
	std::vector<std::function<void()>> callBackOnFinished;
};

REFL_AUTO(type(AssetBase), field(AssetPath), field(isLoadedComplete), field(isBeingLoaded))
