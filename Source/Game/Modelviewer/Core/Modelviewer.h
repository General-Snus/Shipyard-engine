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

class ModelViewer
{
	HINSTANCE myModuleHandle = nullptr;
	HWND myMainWindowHandle = nullptr;

	SplashWindow* mySplashWindow = nullptr;

	ApplicationState myApplicationState;

	Logger MVLogger;
	ModelViewer() = default;

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
	void ExpandWorldBounds(CU::Sphere<float> sphere);
	const CU::Sphere<float>& GetWorldBounds() const;

	// Singleton Getter.
	static ModelViewer& Get()
	{
		static ModelViewer myInstance; return myInstance;
	}

	// Acceleration Getters for components.
	FORCEINLINE static ApplicationState& GetApplicationState()
	{
		return Get().myApplicationState;
	}
	FORCEINLINE static Logger& GetLogger()
	{
		return Get().MVLogger;
	}

	bool Initialize(HINSTANCE aHInstance,SIZE aWindowSize,WNDPROC aWindowProcess,LPCWSTR aWindowTitle);
	int Run();
private: 
	CU::Sphere<float> myWorldBounds;
	GameObject myMesh;
	GameObject myCustomHandler;
	Matrix myModelMatrix;
	std::vector<SaveData<float>> mySaveData;

};
