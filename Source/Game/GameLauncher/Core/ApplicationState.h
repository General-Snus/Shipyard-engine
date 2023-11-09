#pragma once
#include <string>

enum class DebugFilter
{
	NoFilter = 0,
	WorldPosition, 
	VertetexWorldNormal, 
	AlbedoMap,
	NormalMap,
	WorldPixelNormal,
	AmbientOcclusion,
	Roughness,
	Metallic,
	Emission,
	Depth,
	DirectionLight,
	AmbientLight,
	PointLight,
	SpotLight,
	ShadowMap,
	TexCoord0,
	TexCoord1,
	TexCoord2,
	TexCoord3,
	TexCoord4,
	TexCoord5,
	count
};



struct ApplicationState
{
	float CameraSpeed = 1000.0f;
	float CameraRotationSpeed = 50.0f; 
	DebugFilter filter = DebugFilter::NoFilter; 


static inline std::string layerNames[(int)DebugFilter::count] = {
	"NoFilter ",
	"WorldPosition", 
	"VertetexWorldNormal", 
	"AlbedoMap",
	"NormalMap",
	"WorldPixelNormal",
	"AmbientOcclusion",
	"Roughness",
	"Metallic",
	"Emission",
	"Depth",
	"SSAO",
	"AmbientLight",
	"PointLight",
	"SpotLight",
	"ShadowMap",
	"TexCoord0",
	"TexCoord1",
	"TexCoord2",
	"TexCoord3",
	"TexCoord4",
	"TexCoord5",
	};
};