#pragma once 
#include <AssetManager/Objects/Components/Component.h> 
#include <AssetManager/Objects/Components/ComponentDerivatives/DEBUGCOMPONENTS/ImGuiComponent.h>
#include <GraphicsEngine/Rendering/Vertex.h>
#include <ThirdParty/CU/CommonUtills/Vector3.hpp>
#include <ThirdParty/nlohmann/json.hpp> 
 
	class BackgroundColor : public ImGuiComponent
	{
	public:
		BackgroundColor() = delete; // Create a generic cube
		BackgroundColor(const unsigned int anOwnerId); // Create a generic cube   
		BackgroundColor(const unsigned int anOwnerId,CU::Vector4<float> aColor); // Create a generic cube   
		~BackgroundColor() = default;
		void Update() override;
		void SetColor(CU::Vector4<float> aColor);
		void OnExit() override;
		void OnStart() override;
		NLOHMANN_DEFINE_TYPE_INTRUSIVE(BackgroundColor,myOwnerID,myComponentType,myColor.x,myColor.y,myColor.z,myColor.w);

	private:
		CU::Vector4<float> myColor;
	}; 