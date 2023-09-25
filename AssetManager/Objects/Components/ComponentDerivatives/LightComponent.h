#pragma once

#include <AssetManager/AssetManager.pch.h>
#include <AssetManager/AssetManager.h>
#include "../../BaseAssets/LightDataBase.h"


class cLight : public Component
{
public:
	cLight() = delete; // Create a generic cube
	cLight(const unsigned int anOwnerId); // Create a generic cube 
	cLight(const unsigned int anOwnerId,const eLightType type);
	
	eLightType GetType() const;
	void SetType(const eLightType aType);
	 
	std::shared_ptr<LightDataBase> GetData();

	void Update() override;
	void Render() override;

	~cLight() = default;
private:
	std::shared_ptr<LightDataBase> myLightData;
};

