#pragma once
#include <Engine/AssetManager/Objects/Components/Component.h> 
#include <Engine/AssetManager/Objects/Components/ComponentDerivatives/DEBUGCOMPONENTS/ImGuiComponent.h>
#include <Game/Modelviewer/Core/ApplicationState.h>










class RenderMode : public ImGuiComponent
{
public:
	RenderMode() = delete;
	RenderMode(const unsigned int anOwnerId) : ImGuiComponent(anOwnerId) { OnStart(); };
	void Update() override;
	void OnExit() override;
	void OnStart() override;
	~RenderMode() { OnExit(); };

private:
	int currentlyActive = 0;
	bool myRenderMode[(int)DebugFilter::count] = { false };

};