#include "GameState.h"
#include <Tools/Logging/Logging.h>
#include <Windows.h>
#include <libloaderapi.h>

#pragma optimize("", off)

GameState::GameState()
{
}

GameState::GameState(std::filesystem::path pathToProjectFolder)
{
}

bool GameState::AttemptDllLoad()
{
    HMODULE dllHandle = LoadLibrary(L"GameLauncher.dll");
    if (!dllHandle)
    {
        Logger.Err("Failed to load DLL!");
        return false;
    }

    // Get the entry point function from the DLL
    typedef GameLauncher *(*EntryPoint)();
    EntryPoint dllFunction = (EntryPoint)GetProcAddress(dllHandle, "EntrypointMain");
    if (!dllFunction)
    {
        Logger.Err("Failed to get DLL function !");
        FreeLibrary(dllHandle);
        return false;
    }

    m_GameLauncher = dllFunction();
    return true;
}

void GameState::StartPlaySession()
{
    IsLoading = true;
    AttemptDllLoad();
    IsPlaying = true;
}

void GameState::EndPlaySession()
{
    IsPlaying = false;
    IsPaused = false;
}

void GameState::PausePlaySession()
{
    IsPaused = true;
}

void GameState::Init()
{
    if (m_GameLauncher)
    {
        m_GameLauncher->SyncServices(ServiceLocator::Instance());
        m_GameLauncher->Init();
    }
}

void GameState::Start()
{
    if (m_GameLauncher)
    {
        m_GameLauncher->Start();
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
