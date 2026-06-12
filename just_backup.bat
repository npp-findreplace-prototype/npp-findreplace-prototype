@echo off
setlocal EnableExtensions
cd /d "%~dp0"

if exist "git-backup-bundle.bat" (
    call git-backup-bundle.bat
) else (
    echo ERROR: git-backup-bundle.bat was not found.
    echo.
    pause
    exit /b 1
)

endlocal
exit /b %ERRORLEVEL%