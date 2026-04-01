@echo off
SETLOCAL
REM Stop on error
set "ERRORLEVEL=0"

echo Updating submodules...
git submodule update --init --recursive

echo Entering Slang directory...
cd Engine\Libs\Slang

echo Configuring Slang...
cmake --preset vs2022

echo Building Slang (release preset)...
cmake --build --preset release

echo Installing Slang...
cd build
cmake --build . --target install

echo Slang bootstrap complete!
ENDLOCAL
pause