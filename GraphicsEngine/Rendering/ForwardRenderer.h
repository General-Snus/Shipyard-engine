#pragma once



#include "Shaders/Include/Default_VS.h"
#include "Shaders/Include/Default_PS.h"

class ForwardRenderer
{
	ComPtr<ID3D11VertexShader> myVertexShader;
	ComPtr<ID3D11PixelShader> myPixelShader;
	 
public:
	bool Initialize();

};

