#include "AssetManager.pch.h"
#include <Engine/GraphicsEngine/GraphicsEngine.h> 
#include <Tools/ImGui/ImGui/imgui.h>
#include "../BackgroundColor.h"

BackgroundColor::BackgroundColor(const SY::UUID  anOwnerId) : Component(anOwnerId,eComponentType::backgroundColor)
{
}

BackgroundColor::BackgroundColor(const SY::UUID anOwnerId,Vector4f aColor) : Component(anOwnerId,eComponentType::backgroundColor),myColor(aColor)
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
