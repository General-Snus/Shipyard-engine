#pragma once 
#include <Engine/AssetManager/ComponentSystem/Component.h> 
#include <Engine/AssetManager/ComponentSystem/Components/DEBUGCOMPONENTS/ImGuiComponent.h>
#include <Engine/GraphicsEngine/Rendering/Vertex.h>
#include <Tools/Utilities/LinearAlgebra/Vector3.hpp>
#include <Tools/ThirdParty/nlohmann/json.hpp> 

class BackgroundColor : public ImGuiComponent
{
public:
	BackgroundColor() = delete; // Create a generic cube
	BackgroundColor(const unsigned int anOwnerId); // Create a generic cube   
	BackgroundColor(const unsigned int anOwnerId,Vector4f aColor); // Create a generic cube   
	~BackgroundColor() = default;
	void Update() override;
	void SetColor(Vector4f aColor);
	void OnExit() override;
	void OnStart() override;
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(BackgroundColor,myOwnerID,myComponentType,myColor.x,myColor.y,myColor.z,myColor.w);

private:
	Vector4f myColor;
};