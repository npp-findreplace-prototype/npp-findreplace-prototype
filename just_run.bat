@echo off
setlocal EnableExtensions
cd /d "%~dp0"

if not exist "window.exe" (
    echo ERROR: window.exe was not found.
    echo Run just_build.bat first.
    echo.
    pause
    exit /b 1
)

start "" "%CD%\window.exe"

endlocal
exit /b 0