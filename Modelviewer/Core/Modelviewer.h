#pragma once
#include <memory>

#include "ApplicationState.h"
#include "GraphicsEngine/GraphicsEngine.h"
#include "Logging/Logging.h" 
#include <TGAFbx.h>
#include <TgaFbxStructs.h>  
#include <Modelviewer/Windows/SplashWindow.h> 
#include <ThirdParty/CU/CommonUtills/Sphere.hpp>

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

	Logger myLogger;
	ModelViewer() = default;

	void ShowSplashScreen();
	void HideSplashScreen() const; 

	void LoadScene();

	void UpdateScene();

	void Update();

	bool SaveDataToJson();

	bool JsonToSaveData();

	bool ContainData(SaveData<float>& data);

	bool SaveToMemory(eSaveToJsonArgument fnc, std::string identifier, void* arg);

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
		return Get().myLogger;
	}

	bool Initialize(HINSTANCE aHInstance,SIZE aWindowSize,WNDPROC aWindowProcess,LPCWSTR aWindowTitle);
	int Run();
private: 
	CU::Sphere<float> myWorldBounds;
	GameObject myMesh;
	Matrix myModelMatrix;
	std::vector<SaveData<float>> mySaveData;

};
