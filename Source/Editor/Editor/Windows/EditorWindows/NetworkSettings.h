#pragma once
#include "EditorWindow.h"
#include "Tools/Utilities/DataStructures/MathStructs.h"

class NetworkSettings : public EditorWindow {
public:
	NetworkSettings(); 
	void RenderImGUi() override;
	void DrawPingPlot(float newPing); 
private:
	Avg<float, 100> downlink;
	Avg<float, 100> uplink;
}; 
