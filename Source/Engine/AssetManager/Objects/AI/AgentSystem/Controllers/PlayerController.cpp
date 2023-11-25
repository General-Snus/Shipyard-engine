#include <AssetManager.pch.h>
#include "PlayerController.h" 
#include <Tools/Utilities/Input/InputHandler.hpp>

PlayerController::PlayerController()
{
	this->controllerType = eControllerType::player;
}

SteeringOutput PlayerController::Update(GameObject input)
{
	SteeringOutput output;
	auto* transform = input.TryGetComponent<Transform>();

	Input(transform->GetPosition());

	 
	return output;
}

void PlayerController::Input(const Vector3f& currentPosition)
{
	const Vector3f position = currentPosition;
	const Vector3f direction = {0.0f, 0.0f,1.0f};
	Vector3f hitPoint = {0.0f, 0.0f,1.0f}; 
}
 