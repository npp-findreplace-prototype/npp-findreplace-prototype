@echo off
setlocal EnableExtensions EnableDelayedExpansion

cd /d "%~dp0"

echo.
echo === Create Git backup bundle ===
echo.

if not exist ".git" (
    echo ERROR: This folder is not a Git repository.
    pause
    exit /b 1
)

set BACKUP_DIR=%USERPROFILE%\Desktop\findreplacemark-git-backups

if not exist "%BACKUP_DIR%" mkdir "%BACKUP_DIR%"

for /f %%A in ('powershell -NoProfile -Command "Get-Date -Format \"yyyy-MM-dd.HH'h'mm's'ss\""') do set STAMP=%%A

set BACKUP_FILE=%BACKUP_DIR%\findreplacemark-%STAMP%.bundle

git bundle create "%BACKUP_FILE%" --all
if errorlevel 1 (
    echo.
    echo ERROR: Backup bundle failed.
    echo.
    pause
    exit /b 1
)

echo.
echo Backup created:
echo %BACKUP_FILE%
echo.
pause
exit /b 0