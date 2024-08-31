#include "pch.h"
#include "GraphEditorBase.h"
#include "Fonts/RobotoBold.h"
#include "Fonts/RobotoRegular.h"
#include "Fonts/RobotoLightItalic.h"
#include "Fonts/FontAwesomeRegular.h"
#include "Fonts/FontAwesomeSolid.h"

GraphEditorSettingsBase::GraphEditorSettingsBase()
{
	const ImGuiIO& imGuiIO = ImGui::GetIO();
	imGuiIO.Fonts->AddFontDefault();

	NodeBodyFont = imGuiIO.Fonts->AddFontFromMemoryCompressedBase85TTF(RobotoRegular_compressed_data_base85, 18);
	NodeTitleFont = imGuiIO.Fonts->AddFontFromMemoryCompressedBase85TTF(RobotoBold_compressed_data_base85, 18);
	EditorTokenFont = imGuiIO.Fonts->AddFontFromMemoryCompressedBase85TTF(RobotoBold_compressed_data_base85, 64);
	EditorFont = imGuiIO.Fonts->AddFontFromMemoryCompressedBase85TTF(RobotoRegular_compressed_data_base85, 18);

	static constexpr ImWchar iconRanges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
	constexpr float fontAwesomeSize = 16.0f;
	ImFontConfig fontAwesomeConfig;
	fontAwesomeConfig.MergeMode = true;
	fontAwesomeConfig.PixelSnapH = true;
	fontAwesomeConfig.GlyphMinAdvanceX = fontAwesomeSize;
	FontAwesomeRegular = imGuiIO.Fonts->AddFontFromMemoryCompressedBase85TTF(FontAwesomeRegular_compressed_data_base85, fontAwesomeSize, &fontAwesomeConfig, iconRanges);
	FontAwesomeSolid = imGuiIO.Fonts->AddFontFromMemoryCompressedBase85TTF(FontAwesomeSolid_compressed_data_base85, fontAwesomeSize, &fontAwesomeConfig, iconRanges);

}

GraphEditorSettingsBase::~GraphEditorSettingsBase() = default;

GraphEditorStateBase::~GraphEditorStateBase() = default;