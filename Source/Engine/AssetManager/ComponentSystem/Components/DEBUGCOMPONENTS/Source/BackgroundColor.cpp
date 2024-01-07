#include "AssetManager.pch.h"
#include "../BackgroundColor.h"
#include <Engine/GraphicsEngine/GraphicsEngine.h> 
#include <Tools/ImGui/ImGui/imgui.h>

BackgroundColor::BackgroundColor(const SY::UUID  anOwnerId) : ImGuiComponent(anOwnerId,eComponentType::backgroundColor)
{
}

BackgroundColor::BackgroundColor(const SY::UUID anOwnerId,Vector4f aColor) : ImGuiComponent(anOwnerId,eComponentType::backgroundColor), myColor(aColor)
{
}

void BackgroundColor::Update()
{
	GraphicsEngine::Get().GetBackgroundColor() = myColor;
	ImGui::ColorPicker4("BackgroundColor", &myColor.x);
}

void BackgroundColor::SetColor(Vector4f aColor)
{
	myColor = aColor;
}

void BackgroundColor::OnExit()
{
}

void BackgroundColor::OnStart()
{
}
