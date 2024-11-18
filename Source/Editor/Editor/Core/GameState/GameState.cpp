#include "GameState.h"
#include <Editor/Editor/Core/Editor.h>
#include <Engine/PersistentSystems/Scene.h>
#include <Tools/Logging/Logging.h>
#include <Windows.h>
#include <libloaderapi.h> 

GameState::GameState()
{
}

GameState::GameState(std::filesystem::path pathToProjectFolder)
{
}

bool GameState::AttemptDllLoad()
{
    dllHandle = LoadLibrary(L"GameLauncher.dll");
    if (!dllHandle)
    {
        Logger.Err("Failed to load DLL!");
        return false;
    }

    dllFunction = (EntryPoint)GetProcAddress((HMODULE)dllHandle, "EntrypointMain");
	dllFunctionExit = (ExitPoint)GetProcAddress((HMODULE)dllHandle, "ExitPoint");
	if (!(dllFunction && dllFunctionExit))
    {
        Logger.Err("Failed to get DLL function !");
		FreeLibrary((HMODULE)dllHandle);
        return false;
    }

    m_GameLauncher = dllFunction();
    return true;
}

void GameState::StartPlaySession()
{
    if (IsLoading || IsPlaying || IsPaused)
    {
        IsPaused = false;
        IsPlaying = true;
        return;
    }

    m_GameScene = std::make_shared<Scene>("GameScene");

    m_EditorBackupScene = EditorInstance.GetActiveScene();
    m_GameScene->Merge(*m_EditorBackupScene);
    EditorInstance.SetActiveScene(m_GameScene);

    // If you start here, all the copied gameobjects now point to the wrong scene and causes issue
    IsLoading = true;
    AttemptDllLoad();
    IsPlaying = true;
}

void GameState::EndPlaySession()
{

    EditorInstance.SetActiveScene(m_EditorBackupScene);
    if (dllFunction)
    {
        m_GameScene = std::make_shared<Scene>("GameScene");
		try
		{
            Logger.Err("Failed to get DLL function !");
            dllFunction = nullptr;
		}
		catch (const std::exception &e )
		{
			Logger.Critical(e,0);
		}
    }

    if (!IsPlaying)
    {
        return;
    }

    IsPlaying = false;
    IsPaused = false;
}

void GameState::PausePlaySession()
{
    IsPaused = true;
}

void GameState::Init()
{
    try
    {
        if (m_GameLauncher)
        {
            m_GameLauncher->SyncServices(ServiceLocator::Instance());
            m_GameLauncher->Init();
        }
    }
    catch (const std::exception &e)
    {
        IsLoading = false;
        IsPlaying = false;
        Logger.Err(e.what());
    }
}

void GameState::Start()
{
    try
    {
        if (m_GameLauncher)
        {
            m_GameLauncher->Start();
        }
    }
    catch (const std::exception &e)
    {
        IsLoading = false;
        IsPlaying = false;
        Logger.Err(e.what());
    }
}

void GameState::Update(float delta)
{
    try
    {
        if (IsLoading)
        {
            IsLoading = false;
            Init();
            Start();
        }

        if (m_GameLauncher && !IsPaused && IsPlaying)
        {
            m_GameLauncher->Update(delta);
        }
    }
    catch (const std::exception &e)
    {
        IsLoading = false;
        IsPlaying = false;
        Logger.Err(e.what());
    }
}
