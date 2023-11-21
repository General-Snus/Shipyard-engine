#pragma once
#include <Engine/AssetManager/ComponentSystem/Component.h> 
#include <Engine/AssetManager/ComponentSystem/Components/DEBUGCOMPONENTS/ImGuiComponent.h>
#include <Editor/Editor/Core/ApplicationState.h> 

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
	int currentlyActiveTone = 0;
	int currentlyActiveLayer = 0;
	bool myRenderMode[(int)DebugFilter::count] = { false };

};