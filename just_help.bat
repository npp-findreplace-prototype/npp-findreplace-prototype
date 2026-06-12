@echo off
setlocal EnableExtensions
cd /d "%~dp0"

echo.
echo ============================================================
echo  just_* helper commands - quick summary
echo ============================================================
echo.
echo just_help.bat             - Show this help.
echo just_build.bat            - Build only. No commit. No push.
echo just_commit.bat           - Commit current local changes only. No push.
echo just_push.bat             - Push already-created local commits to GitHub.
echo just_compileandrun.bat    - Build only, then run window.exe if build succeeds.
echo just_run.bat              - Run the current existing window.exe.
echo just_status.bat           - Show Git/project status and recommended next action.
echo just_check.bat            - Check build setup only. Does not compile.
echo just_getlatest.bat        - Safely get latest commits from GitHub.
echo just_history.bat          - Show recent Git commit history.
echo just_diff.bat             - Show changed files and a diff summary.
echo just_backup.bat           - Create an offline Git backup bundle.
echo just_verifygithub.bat     - Test that GitHub can be cloned successfully.
echo.
echo Main everyday choices:
echo   build.bat               - Build, commit, and push.
echo   just_build.bat          - Build only.
echo   just_compileandrun.bat  - Build only, then run.
echo   just_status.bat         - Use when unsure.
echo.
pause

echo.
echo ============================================================
echo  More usage help
echo ============================================================
echo.

echo ------------------------------------------------------------
echo just_build.bat
echo ------------------------------------------------------------
echo Use when:
echo   You want to compile-test the project.
echo   You do not want a Git commit.
echo   You do not want a GitHub push.
echo.
echo Equivalent to:
echo   build.bat nosync
echo.
echo Notes:
echo   It still creates local build_... and source_... folders
echo   after a successful build.
echo.

echo ------------------------------------------------------------
echo just_commit.bat
echo ------------------------------------------------------------
echo Use when:
echo   You changed docs, helper scripts, README, comments, or config.
echo   You want to save those changes locally in Git.
echo   You do not want to push yet.
echo.
echo It does:
echo   git add --all
echo   git commit
echo.
echo It does not:
echo   build
echo   push
echo.
echo Afterward, push later with:
echo   just_push.bat
echo.

echo ------------------------------------------------------------
echo just_push.bat
echo ------------------------------------------------------------
echo Use when:
echo   You already have local commits.
echo   You want to send them to GitHub.
echo.
echo It does:
echo   git push
echo.
echo It does not:
echo   build
echo   commit new file changes
echo.
echo If push fails, run:
echo   just_status.bat
echo.

echo ------------------------------------------------------------
echo just_compileandrun.bat
echo ------------------------------------------------------------
echo Use when:
echo   You want to test the program quickly.
echo   You want it to build first.
echo   You do not want commit or push.
echo.
echo It does:
echo   build.bat nosync
echo   start window.exe
echo.
echo If the build fails:
echo   It does not run the program.
echo.

echo ------------------------------------------------------------
echo just_run.bat
echo ------------------------------------------------------------
echo Use when:
echo   You only want to run the last built window.exe.
echo   You do not need to rebuild.
echo.
echo It does not:
echo   compile
echo   commit
echo   push
echo.

echo ------------------------------------------------------------
echo just_status.bat
echo ------------------------------------------------------------
echo Use when:
echo   You are unsure what to do.
echo   You want to know whether files changed.
echo   You want to know whether GitHub is ahead or behind.
echo.
echo Typical recommendations:
echo   Local file changes: run build.bat or just_commit.bat.
echo   Ahead of GitHub: run just_push.bat.
echo   Behind GitHub: run just_getlatest.bat.
echo.

echo ------------------------------------------------------------
echo just_check.bat
echo ------------------------------------------------------------
echo Use when:
echo   You want to check setup without compiling.
echo   You want to verify TCC, PowerShell, Robocopy, source files, and Git.
echo.
echo Equivalent to:
echo   build.bat check
echo.
echo It does not:
echo   compile
echo   create snapshots
echo   commit
echo   push
echo.

echo ------------------------------------------------------------
echo just_getlatest.bat
echo ------------------------------------------------------------
echo Use when:
echo   You are starting work on another computer.
echo   GitHub has newer commits.
echo   just_status.bat says you are behind GitHub.
echo.
echo It should only update when your local folder is clean.
echo Do not use it to overwrite local edits.
echo.

echo ------------------------------------------------------------
echo just_backup.bat
echo ------------------------------------------------------------
echo Use when:
echo   You want an extra offline backup.
echo   You are about to do risky work.
echo   You are moving between computers.
echo.
echo It creates a Git bundle backup on your Desktop.
echo It does not need internet.
echo.

echo ------------------------------------------------------------
echo just_verifygithub.bat
echo ------------------------------------------------------------
echo Use when:
echo   You want proof that GitHub has a cloneable copy.
echo   You made an important push.
echo   You want to test before relying on another computer.
echo.
echo It creates a temporary clone, shows it, then removes it.
echo It needs internet and GitHub access.
echo.

echo ------------------------------------------------------------
echo Suggested workflows
echo ------------------------------------------------------------
echo Normal code save:
echo   build.bat
echo.
echo Quick compile test:
echo   just_build.bat
echo.
echo Quick compile and run:
echo   just_compileandrun.bat
echo.
echo Docs/scripts only:
echo   just_commit.bat
echo   just_push.bat
echo.
echo Starting on another computer:
echo   just_getlatest.bat
echo.
echo Unsure:
echo   just_status.bat
echo.

echo ============================================================
echo.
pause
exit /b 0