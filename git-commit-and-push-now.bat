@echo off
setlocal EnableExtensions EnableDelayedExpansion

cd /d "%~dp0"

echo.
echo === Manual commit and push ===
echo.

if not exist ".git" (
    echo ERROR: This folder is not a Git repository.
    pause
    exit /b 1
)

set DIRTY=
for /f "delims=" %%A in ('git status --porcelain') do set DIRTY=1

if not defined DIRTY (
    echo No local file changes to commit.
    echo.
    echo Trying push anyway, in case commits are pending...
    git push
    echo.
    pause
    exit /b 0
)

echo Current changes:
git status --short
echo.

set /p MSG=Commit message, or press Enter for default: 

if "%MSG%"=="" set MSG=Manual save %DATE% %TIME%

git add --all
if errorlevel 1 (
    echo ERROR: git add failed.
    pause
    exit /b 1
)

git commit -m "%MSG%"
if errorlevel 1 (
    echo ERROR: git commit failed.
    pause
    exit /b 1
)

git push
if errorlevel 1 (
    echo.
    echo ERROR: Push failed.
    echo Your commit is saved locally, but not pushed to GitHub.
    echo Run git-status-check.bat.
    echo.
    pause
    exit /b 1
)

echo.
echo Commit and push complete.
echo.
pause
exit /b 0