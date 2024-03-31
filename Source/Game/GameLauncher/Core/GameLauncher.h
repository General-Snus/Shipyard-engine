#pragma once 
#include  <Engine/AssetManager/ComponentSystem/GameObject.h>
#include <Tools/Logging/Logging.h>
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.h>
class GameLauncher
{
public:
	GameLauncher() = default;
	void Init();
	void GenerateNewRandomCubes();
	void Start();
	void Update(float delta);
private:
	GameObject myMesh;
	GameObject myCustomHandler;
	Matrix myModelMatrix;
	float direction = 1.f;
	std::vector<GameObject> vectorOfGameObjects;

};
