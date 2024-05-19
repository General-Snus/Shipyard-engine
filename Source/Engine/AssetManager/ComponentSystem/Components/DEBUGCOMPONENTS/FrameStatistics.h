#pragma once
#include <Engine/AssetManager/ComponentSystem/Component.h>  

class FrameStatistics : public Component
{
public:
	FrameStatistics() = delete;
	FrameStatistics(const SY::UUID anOwnerId);
	void Update() override;

	~FrameStatistics() override {  };

private:
#define MAX_DATA_COUNT 10
	int myDataIndex = 0;
	float myAverageFPS[MAX_DATA_COUNT] = { 0.0f };
	float myAverageVirtualMemoryByApp[MAX_DATA_COUNT] = { 0.0f };
	float myAverageMemoryByApp[MAX_DATA_COUNT] = { 0.0f };
	float myAverageCPU[MAX_DATA_COUNT] = { 0.0f };
	float myAverageCPUByApp[MAX_DATA_COUNT] = { 0.0f };

};
REFL_AUTO(type(FrameStatistics))