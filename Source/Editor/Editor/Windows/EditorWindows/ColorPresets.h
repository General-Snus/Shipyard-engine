#pragma once
#include "EditorWindow.h"
#include <string>  
#include <vector>   
#include "Tools/Utilities/LinearAlgebra/Vector4.hpp"

class ColorPresets : public EditorWindow
{
public:
	void RenderImGUi() override;

private:
	std::string activeColorAdd{};
	Vector4f activeColor{};
	bool blendColor{};
	float blendFactor{};
};

