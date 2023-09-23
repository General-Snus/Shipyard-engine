#include "AssetManager.pch.h"
#include "BackgroundColor.h"
#include <GraphicsEngine/GraphicsEngine.h> 
#include <ImGui/imgui.h>

BackgroundColor::BackgroundColor(const unsigned int anOwnerId) : ImGuiComponent(anOwnerId,eComponentType::backgroundColor)
{
}

BackgroundColor::BackgroundColor(const unsigned int anOwnerId, CU::Vector4<float> aColor) : ImGuiComponent(anOwnerId,eComponentType::backgroundColor), myColor(aColor)
{
}

void BackgroundColor::Update()
{
	GraphicsEngine::Get().GetBackgroundColor() = myColor;
	ImGui::ColorPicker4("BackgroundColor", &myColor.x);
}

void BackgroundColor::SetColor(CU::Vector4<float> aColor)
{
	myColor = aColor;
}

void BackgroundColor::OnExit()
{
}

void BackgroundColor::OnStart()
{
}
