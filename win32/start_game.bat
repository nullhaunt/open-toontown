@echo off
title Open Toontown - Game Client
cd..

set PYTHONPATH=%CD%\Native\Binary;%PYTHONPATH%

set /P PPYTHON_PATH=<PPYTHON_PATH

set /P LOGIN_TOKEN=Login token [dev]: 
if "%LOGIN_TOKEN%"=="" set LOGIN_TOKEN=dev

%PPYTHON_PATH% -m toontown.launcher.QuickStartLauncher
pause