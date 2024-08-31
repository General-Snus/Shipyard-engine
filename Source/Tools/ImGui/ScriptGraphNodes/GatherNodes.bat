@echo off
if "%~1"=="" GOTO ERROR

setlocal EnableDelayedExpansion
setlocal

set "parampath=%~f1\"
set "parampath=%parampath:\\=\%"

set /a count=0
(
    echo #pragma once
    for /R "%parampath%" %%F in (*.h) do (
        set "_dir=%%~dpF"
        if not [%cd%\] equ [!_dir!] (
            set "folder=%%~fF"
            call set folder=%%folder:%parampath%^=%%
            echo #include "!folder!"
            set /a count=count+1
        )
    )
)>%parampath%NodeIncludes.h
echo %count% Nodes registered.
exit /B 0

:ERROR
echo Failed to run!
exit /B -1