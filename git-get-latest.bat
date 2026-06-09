@echo off
setlocal EnableExtensions EnableDelayedExpansion

cd /d "%~dp0"

echo.
echo === Get latest from GitHub ===
echo.

if not exist ".git" (
    echo ERROR: This folder is not a Git repository.
    pause
    exit /b 1
)

set DIRTY=
for /f "delims=" %%A in ('git status --porcelain') do set DIRTY=1

if defined DIRTY (
    echo ERROR: You have local changes.
    echo.
    echo Run git-status-check.bat first.
    echo Do not pull over local work.
    echo.
    git status --short
    echo.
    pause
    exit /b 1
)

echo Fetching from GitHub...
git fetch
if errorlevel 1 (
    echo.
    echo ERROR: Fetch failed.
    echo Check internet or GitHub login.
    echo.
    pause
    exit /b 1
)

git rev-parse --abbrev-ref --symbolic-full-name @{u} >nul 2>nul
if errorlevel 1 (
    echo.
    echo ERROR: No upstream branch configured.
    echo.
    pause
    exit /b 1
)

set AHEAD=0
set BEHIND=0

for /f "tokens=1,2" %%A in ('git rev-list --left-right --count HEAD...@{u}') do (
    set AHEAD=%%A
    set BEHIND=%%B
)

if %AHEAD% GTR 0 if %BEHIND% GTR 0 (
    echo.
    echo ERROR: Local repo and GitHub have both changed.
    echo Do not auto-merge. Ask before doing anything.
    echo.
    pause
    exit /b 1
)

if %BEHIND% GTR 0 (
    echo Updating local repo...
    git merge --ff-only @{u}
    if errorlevel 1 (
        echo.
        echo ERROR: Update failed.
        echo.
        pause
        exit /b 1
    )
    echo.
    echo Updated successfully.
    pause
    exit /b 0
)

if %AHEAD% GTR 0 (
    echo.
    echo You already have local commits not on GitHub.
    echo Run:
    echo   git-push-local.bat
    echo.
    pause
    exit /b 0
)

echo.
echo Already up to date.
echo.
pause
exit /b 0