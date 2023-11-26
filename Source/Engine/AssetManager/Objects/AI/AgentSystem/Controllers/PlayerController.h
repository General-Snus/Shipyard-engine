#pragma once
#include "Controller.h" 


class PlayerController : public Controller
{
public:
	explicit PlayerController();
	bool Update(GameObject input) override;

private:
	void Input(const Vector3f& currentPosition); 
	Vector3f end; 
};