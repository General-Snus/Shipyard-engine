#pragma once
#include "EditorWindow.h"
#include "Tools/Utilities/LinearAlgebra/Vector4.hpp"
#include <string>  
#include <vector>   

class ColorPresets : public EditorWindow
{
public:
	ColorPresets() : EditorWindow("ColorPresets") {}
	void RenderImGUi() override;

private:
	std::string activeColorAdd{};
	Vector4f activeColor{};
	bool blendColor{};
	float blendFactor{};
};

