@echo off
setlocal EnableExtensions EnableDelayedExpansion

cd /d "%~dp0"

echo.
echo === Git status check ===
echo Folder: %CD%
echo.

if not exist ".git" (
    echo ERROR: This folder is not a Git repository.
    echo.
    pause
    exit /b 1
)

where git >nul 2>nul
if errorlevel 1 (
    echo ERROR: git was not found.
    echo.
    pause
    exit /b 1
)

echo Branch:
git branch --show-current
echo.

echo Remote:
git remote -v
echo.

echo Local status:
git status -sb
echo.

echo Checking GitHub...
git fetch --quiet
if errorlevel 1 (
    echo.
    echo WARNING: Could not contact GitHub.
    echo This may be internet/login related.
    echo Local status above is still useful.
    echo.
    pause
    exit /b 1
)

git rev-parse --abbrev-ref --symbolic-full-name @{u} >nul 2>nul
if errorlevel 1 (
    echo.
    echo No upstream branch is configured.
    echo Usually fix with:
    echo   git push -u origin main
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

set DIRTY=
for /f "delims=" %%A in ('git status --porcelain') do set DIRTY=1

echo.
echo === Recommendation ===

if defined DIRTY (
    echo You have local file changes.
    echo.
    echo Usually:
    echo   Run build.bat
    echo.
    echo If the build succeeds, build.bat should commit and push for you.
) else (
    if %AHEAD% EQU 0 if %BEHIND% EQU 0 (
        echo Everything is clean and synced.
        echo No action needed.
    )

    if %AHEAD% GTR 0 if %BEHIND% EQU 0 (
        echo You have local commits that are not on GitHub yet.
        echo Run:
        echo   git-push-local.bat
    )

    if %AHEAD% EQU 0 if %BEHIND% GTR 0 (
        echo GitHub has newer commits.
        echo Run:
        echo   git-get-latest.bat
    )

    if %AHEAD% GTR 0 if %BEHIND% GTR 0 (
        echo Your local repo and GitHub have both changed.
        echo Do not guess. Ask before doing anything.
    )
)

echo.
echo Ahead of GitHub:  %AHEAD%
echo Behind GitHub:   %BEHIND%
echo.

pause
exit /b 0