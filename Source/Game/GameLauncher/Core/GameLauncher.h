#pragma once 
#include "Engine/AssetManager/ComponentSystem/GameObject.h"  
 
class GameLauncher
{
public:
	GameLauncher() = default;
	void Init();
	void GenerateNewRandomCubes();
	void Start();
	void Update(float delta);
private:

	GameObject myCustomHandler;
	GameObject myCustomHandler2; 
	float direction = 1.f;
	float direction2 = -1.f;
	std::vector<GameObject> vectorOfGameObjects;

};
