#pragma once
#include <memory>
#include "ApplicationState.h"
#include "../Windows/SplashWindow.h" 
#include <Tools/Logging/Logging.h> 
#include <Tools/Utilities/LinearAlgebra/Sphere.hpp>
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.hpp> 
#include <Engine/AssetManager/Objects/GameObjects/GameObject.h>

template<typename T>
struct SaveData
{
	int fnc;
	std::string identifier;
	T* arg;
};


enum class eSaveToJsonArgument
{
	InputFloat3,
	InputFloat4,
	SaveBool
}; 

class Editor
{
private:
	SplashWindow* mySplashWindow = nullptr;
	ApplicationState myApplicationState;
	Logger MVLogger;
	void ShowSplashScreen();
	void HideSplashScreen() const; 
	void LoadScene();
	void UpdateScene();
	void Update();
	bool SaveDataToJson() const;
	bool JsonToSaveData() const; 
	bool ContainData(SaveData<float>& data);
	bool SaveToMemory(eSaveToJsonArgument fnc,const std::string& identifier, void* arg);
	bool SaveToMemory(SaveData<float>& arg);

public:
	Editor() = default;
	bool Initialize(HWND aHandle);
	void DoWinProc(const MSG& msg);
	int Run();
	 
	 
	void ExpandWorldBounds(CU::Sphere<float> sphere);
	const CU::Sphere<float>& GetWorldBounds() const;
	// Acceleration Getters for components.
	FORCEINLINE ApplicationState& GetApplicationState()
	{
		return myApplicationState;
	}
	FORCEINLINE Logger& GetLogger()
	{
		return MVLogger;
	}
private: 
	CU::Sphere<float> myWorldBounds;
	GameObject myMesh;
	GameObject myCustomHandler;
	Matrix myModelMatrix;
	std::vector<SaveData<float>> mySaveData;

};
