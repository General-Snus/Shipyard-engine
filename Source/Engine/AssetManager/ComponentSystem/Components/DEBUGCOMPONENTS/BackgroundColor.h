#pragma once 
#include <Engine/AssetManager/ComponentSystem/Component.h>  
#include <Engine/GraphicsEngine/Rendering/Vertex.h>
#include <Tools/ThirdParty/nlohmann/json_fwd.hpp> 
#include <Tools/Utilities/LinearAlgebra/Vector3.hpp>

class BackgroundColor : public Component
{
public:
	BackgroundColor() = delete; // Create a generic cube
	BackgroundColor(const SY::UUID anOwnerId,GameObjectManager* aManager); // Create a generic cube   
	BackgroundColor(const SY::UUID anOwnerId,GameObjectManager* aManager,Vector4f aColor); // Create a generic cube   
	~BackgroundColor() override = default;
	void Update() override;
	void SetColor(Vector4f aColor);
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(BackgroundColor,myOwnerID,myComponentType,myColor.x,myColor.y,myColor.z,myColor.w);

private:
	Vector4f myColor;
};
REFL_AUTO(type(BackgroundColor))