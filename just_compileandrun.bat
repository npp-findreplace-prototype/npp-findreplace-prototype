@echo off
setlocal EnableExtensions
cd /d "%~dp0"

echo.
echo === Build and run ===
echo.

call build.bat nosync
if errorlevel 1 (
    echo.
    echo Build failed. Not running.
    echo.
    pause
    exit /b 1
)

if not exist "window.exe" (
    echo.
    echo ERROR: window.exe was not found after build.
    echo.
    pause
    exit /b 1
)

echo.
echo Running window.exe...
echo.

start "" "%CD%\window.exe"

endlocal
exit /b 0