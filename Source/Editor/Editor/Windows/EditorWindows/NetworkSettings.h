#pragma once
#include "EditorWindow.h"
#include "Tools/Utilities/DataStructures/MathStructs.h"

class NetworkSettings : public EditorWindow {
public:
	NetworkSettings(); 
	void RenderImGUi() override;
	void DrawPingPlot(float newPing); 
private:
	Avg<float, 10000> downlink;
	Avg<float, 10000> uplink;
}; 
