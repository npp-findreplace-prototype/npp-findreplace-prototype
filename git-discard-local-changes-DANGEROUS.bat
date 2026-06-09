@echo off
setlocal EnableExtensions

cd /d "%~dp0"

echo.
echo === DANGEROUS: Discard local uncommitted changes ===
echo.

if not exist ".git" (
    echo ERROR: This folder is not a Git repository.
    pause
    exit /b 1
)

echo This will:
echo - restore tracked files to the last commit
echo - delete untracked non-ignored files
echo.
echo It should not delete ignored build folders, oldbuilds, temp, or exe files.
echo.
echo Current changes:
git status --short
echo.

set /p CONFIRM=Type YES to discard local changes: 

if not "%CONFIRM%"=="YES" (
    echo Cancelled.
    pause
    exit /b 0
)

git restore .
if errorlevel 1 (
    echo ERROR: git restore failed.
    pause
    exit /b 1
)

git clean -fd
if errorlevel 1 (
    echo ERROR: git clean failed.
    pause
    exit /b 1
)

echo.
echo Local uncommitted changes discarded.
echo.
git status
echo.
pause
exit /b 0