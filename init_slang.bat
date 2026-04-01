@echo off
SETLOCAL

:: Checking for admin rights
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo Requesting admin rights...
    powershell -Command "Start-Process '%~f0' -Verb RunAs"
    exit
)

REM Stop on error
set "ERRORLEVEL=0"

echo Updating submodules...
git submodule update --init --recursive

echo Entering Slang directory...
cd /d "%~dp0Engine\Libs\Slang"

echo Please select a CMake Preset:
echo 1 - vs2022
echo 2 - vs2026

choice /c 12 /n /m "Your Choice (1 or 2) : "

if errorlevel 2 (
    set "CMAKE_PRESET=vs2026"
) else (
    set "CMAKE_PRESET=vs2022"
)

echo Preset chosen : %CMAKE_PRESET%
echo.

echo Configuring Slang...
cmake --preset %CMAKE_PRESET%

echo Building Slang (release preset)...
cmake --build --preset release

echo Installing Slang...
cd build
cmake --build . --target install

echo Slang bootstrap complete!
ENDLOCAL
pause