#include "AssetManager.pch.h"
#include "LightComponent.h"

cLight::cLight(const unsigned int anOwnerId) : Component(anOwnerId)
{
	myLightData = std::make_shared<LightDataBase>();
}
cLight::cLight(const unsigned int anOwnerId,const eLightType type) : Component(anOwnerId)
{
	switch(type)
	{
	case eLightType::Directional:
		myLightData = std::make_shared<DirectionalLight>();
		break;

	case eLightType::Point:
		myLightData = std::make_shared<PointLight>();
		break;

	case eLightType::Spot:
		myLightData = std::make_shared<SpotLight>();
		break;

	default:
		myLightData = std::make_shared<LightDataBase>();
		break;
	}
}
eLightType cLight::GetType() const
{
	return  myLightData->LightType;
}
void cLight::SetType(const eLightType aType)
{
	switch(aType)
	{
	case eLightType::Directional:
		myLightData = std::make_shared<DirectionalLight>();
		break;
	case eLightType::Point:
		myLightData = std::make_shared<PointLight>();
		break;
	case eLightType::Spot:
		myLightData = std::make_shared<SpotLight>();
		break;
	default:
		myLightData = std::make_shared<LightDataBase>();
		break;
	}
} 
std::shared_ptr<LightDataBase> cLight::GetData()
{
	return  ( myLightData);
}
 
void cLight::Update()
{
}
void cLight::Render()
{ 
}
