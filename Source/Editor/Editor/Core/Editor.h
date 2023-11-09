#pragma once
#include <vector>
#include "ApplicationState.h"
#include "../Windows/SplashWindow.h" 
#include <Tools/Logging/Logging.h> 
#include <Tools/Utilities/LinearAlgebra/Sphere.hpp>
#include <Game/GameLauncher/Core/GameLauncher.h>
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
class GameLauncher;

class Editor
{
private:
	SplashWindow* mySplashWindow = nullptr;
	ApplicationState myApplicationState;
	Logger MVLogger;
	void ShowSplashScreen();
	void HideSplashScreen() const;  

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
	 
	 
	void ExpandWorldBounds(Sphere<float> sphere);
	const Sphere<float>& GetWorldBounds() const;
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
	GameLauncher myGameLauncher;
	Sphere<float> myWorldBounds; 
	std::vector<SaveData<float>> mySaveData; 
};
