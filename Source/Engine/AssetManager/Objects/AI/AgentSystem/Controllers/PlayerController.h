#pragma once
#include "Controller.h" 


class PlayerController : public Controller
{
public:
	MYLIB_REFLECTABLE()
	explicit PlayerController();
	bool Update(GameObject input) override;

private:
	void InputPosition(const Vector3f& currentPosition); 
	Vector3f end; 
};
REFL_AUTO(type(PlayerController))