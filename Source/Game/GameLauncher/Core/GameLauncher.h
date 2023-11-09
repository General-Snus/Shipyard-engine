#pragma once
#include <Engine/AssetManager/Objects/GameObjects/GameObject.h>
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.hpp> 

class Logger;
class GameLauncher
{
public: 
	GameLauncher() = default; 
	void Init();
	void Start(); 
	void Update(); 
	Logger GLLogger;
private:  
	GameObject myMesh;
	GameObject myCustomHandler;
	Matrix myModelMatrix; 

};
