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

	void UpdateImGui(); 
	void Update(); 
	void Render();

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
	 
	 
	// Acceleration Getters for components.
	static ApplicationState& GetApplicationState()
	{
		static ApplicationState myApplicationState;
		return myApplicationState;
	}

	static Editor& GetEditor() // HATE
	{
		static Editor aEditorInstance;
		return aEditorInstance;
	}
	FORCEINLINE Logger& GetLogger()
	{
		return MVLogger;
	}

	static RECT GetViewportRECT();
	static Vector2<int> GetViewportResolution();

	void ExpandWorldBounds(Sphere<float> sphere);
	const Sphere<float>& GetWorldBounds();
private: 
	Sphere<float> myWorldBounds;
	GameLauncher myGameLauncher;
	std::vector<SaveData<float>> mySaveData; 
};
