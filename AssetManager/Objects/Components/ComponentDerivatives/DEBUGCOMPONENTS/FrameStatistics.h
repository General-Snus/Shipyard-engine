#pragma once
#include <AssetManager/Objects/Components/Component.h> 
#include <AssetManager/Objects/Components/ComponentDerivatives/DEBUGCOMPONENTS/ImGuiComponent.h>

class FrameStatistics : public ImGuiComponent
{
public:
	FrameStatistics() = delete;
	FrameStatistics(const unsigned int anOwnerId) : ImGuiComponent(anOwnerId) { OnStart(); };
	void Update() override;
	void OnExit() override;
	void OnStart() override;

	~FrameStatistics() { OnExit(); };

private:
#define MAX_DATA_COUNT 10
	int myDataIndex = 0;
	float myAverageFPS[MAX_DATA_COUNT] = {0.0f};
	float myAverageVirtualMemoryByApp[MAX_DATA_COUNT] = {0.0f};
	float myAverageMemoryByApp[MAX_DATA_COUNT] = {0.0f};
	float myAverageCPU[MAX_DATA_COUNT] = {0.0f};
	float myAverageCPUByApp[MAX_DATA_COUNT] = {0.0f};

};