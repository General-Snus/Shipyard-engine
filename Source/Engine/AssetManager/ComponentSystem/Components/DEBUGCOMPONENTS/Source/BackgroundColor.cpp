#include "Engine/AssetManager/AssetManager.pch.h"
#include <Engine/GraphicsEngine/GraphicsEngine.h> 
#include <Tools/ImGui/ImGui/imgui.h>
#include "../BackgroundColor.h"

BackgroundColor::BackgroundColor(const SY::UUID  anOwnerId,GameObjectManager* aManager) : Component(anOwnerId,aManager,eComponentType::backgroundColor)
{
}

BackgroundColor::BackgroundColor(const SY::UUID anOwnerId,GameObjectManager* aManager,Vector4f aColor) : Component(anOwnerId,aManager,eComponentType::backgroundColor),myColor(aColor)
{
}

void BackgroundColor::Update()
{
	GraphicsEngine::Get().GetBackgroundColor() = myColor;
	ImGui::ColorPicker4("BackgroundColor",&myColor.x);
}

void BackgroundColor::SetColor(Vector4f aColor)
{
	myColor = aColor;
}
