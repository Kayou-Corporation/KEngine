@echo off
SETLOCAL
REM Stop on error
set "ERRORLEVEL=0"

echo Updating submodules...
git submodule update --init --recursive

echo Submodules updated successfully!
ENDLOCAL
pause