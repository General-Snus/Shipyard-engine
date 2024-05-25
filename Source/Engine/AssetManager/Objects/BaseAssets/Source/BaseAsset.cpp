#include "AssetManager.pch.h"

#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>
#include <wrl.h>
#include "../BaseAsset.h"

#include "Engine/GraphicsEngine/GraphicsEngine.h"
#include "Tools/ImGui/ImGui/ImGuiHepers.hpp"

AssetBase::AssetBase(const std::filesystem::path& aFilePath) : AssetPath(aFilePath),isLoadedComplete(false)
{
}

inline void AssetBase::InspectorView()
{
	Vector4f color; //Color asset status indicator
	if (isLoadedComplete)
	{
		color = Vector4f(0,1,0,1);
	}
	else if (isBeingLoaded)
	{
		color = Vector4f(1,1,0,1);
	}
	else
	{
		color = Vector4f(1,0,0,1);
	}

	const TypeInfo& typeInfo = this->GetTypeInfo(); // get custom type info

	float size = ImGui::CalcTextSize("A").y;
	ImGui::Image(GraphicsEngine::Get().GetDefaultTexture(eTextureType::ColorMap),ImVec2(size,size),ImVec2(0,0),ImVec2(1,1),color);
	ImGui::SameLine();
	ImGui::Text(typeInfo.Name().c_str());
	ImGui::Text((std::string("AssetPath: ") + AssetPath.string()).c_str());

}
