#pragma once 
#include "../../Component.h"

class Actor;
class cActor : public Component
{
public:
	cActor(const unsigned int anOwnerID);
	~cActor() = default;  
	void Init() override;
	void Update() override;
	void Render() override; 
	Actor* GetActor(); 
private:
	Actor* myActor = nullptr;
};

