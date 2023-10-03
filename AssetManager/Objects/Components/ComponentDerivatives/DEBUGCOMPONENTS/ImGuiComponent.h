#pragma once
#include <AssetManager/Objects/Components/Component.h>
class ImGuiComponent : public Component 
{  
public:
	ImGuiComponent(const unsigned int anOwnerID) : Component(anOwnerID) {  }
	ImGuiComponent(const unsigned int anOwnerID,eComponentType aComponentType) : Component(anOwnerID,aComponentType) {  }
	~ImGuiComponent() { } 

	virtual void OnStart()  {};
	virtual void OnExit()   {};
	virtual void OnChange() {};
};

