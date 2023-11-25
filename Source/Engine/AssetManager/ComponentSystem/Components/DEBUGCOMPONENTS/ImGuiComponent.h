#pragma once
#include <Engine/AssetManager/ComponentSystem/Component.h>

class ImGuiComponent : public Component 
{  
public:
	ImGuiComponent(const SY::UUID anOwnerID) : Component(anOwnerID) {  }
	ImGuiComponent(const SY::UUID anOwnerID,eComponentType aComponentType) : Component(anOwnerID,aComponentType) {  }
	~ImGuiComponent() { } 

	virtual void OnStart()  {};
	virtual void OnExit()   {};
	virtual void OnChange() {};
};

