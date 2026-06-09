@echo off
setlocal EnableExtensions

cd /d "%~dp0"

echo.
echo ============================================================
echo  FindReplaceMark Git helper scripts
echo ============================================================
echo.
echo Put these .bat files in the project folder:
echo   %CD%
echo.
echo Normal daily workflow:
echo   1. Edit files
echo   2. Run build.bat
echo   3. If the build succeeds, it commits and pushes to GitHub
echo.
echo Safe first check:
echo   Run git-status-check.bat whenever you are unsure.
echo.
echo ------------------------------------------------------------
echo  build.bat
echo ------------------------------------------------------------
echo What it does:
echo   Builds the program.
echo   Creates build/source snapshots only after a successful build.
echo   Commits source changes to Git.
echo   Pushes the commit to GitHub.
echo.
echo When to use it:
echo   Use this after normal code edits.
echo   This is the main everyday command.
echo.
echo Notes:
echo   Failed builds are not pushed.
echo   Ignored build folders and exe files are not uploaded to GitHub.
echo.
echo ------------------------------------------------------------
echo  git-status-check.bat
echo ------------------------------------------------------------
echo What it does:
echo   Shows the current Git branch.
echo   Shows the GitHub remote.
echo   Shows whether files are changed locally.
echo   Checks whether your computer is ahead of or behind GitHub.
echo   Prints a recommendation for what to do next.
echo.
echo When to use it:
echo   Use this whenever you are unsure.
echo   Use this before pulling, pushing, or switching computers.
echo.
echo ------------------------------------------------------------
echo  git-get-latest.bat
echo ------------------------------------------------------------
echo What it does:
echo   Safely gets the latest version from GitHub.
echo   It only updates if your local folder has no uncommitted changes.
echo   It uses a fast-forward update only.
echo.
echo When to use it:
echo   Use this before starting work on another computer.
echo   Use this if git-status-check.bat says GitHub has newer commits.
echo.
echo Do not use it when:
echo   You have local file changes.
echo   Your local repo and GitHub have both changed.
echo.
echo ------------------------------------------------------------
echo  git-push-local.bat
echo ------------------------------------------------------------
echo What it does:
echo   Pushes local commits to GitHub.
echo   It does not create a new commit.
echo.
echo When to use it:
echo   Use this if git-status-check.bat says you have local commits
echo   that are not on GitHub yet.
echo.
echo Notes:
echo   If you edited files but did not commit them, run build.bat
echo   or git-commit-and-push-now.bat instead.
echo.
echo ------------------------------------------------------------
echo  git-commit-and-push-now.bat
echo ------------------------------------------------------------
echo What it does:
echo   Commits current file changes.
echo   Asks you for a commit message.
echo   Pushes the commit to GitHub.
echo.
echo When to use it:
echo   Use this for README edits, docs, helper scripts, comments,
echo   or other non-build changes.
echo.
echo For code changes:
echo   Prefer build.bat, because build.bat only commits after
echo   a successful build.
echo.
echo ------------------------------------------------------------
echo  git-show-history.bat
echo ------------------------------------------------------------
echo What it does:
echo   Shows recent Git commit history.
echo   Displays commit IDs, branches, and graph lines.
echo.
echo When to use it:
echo   Use this to see recent saves.
echo   Use this to confirm that builds or manual commits were saved.
echo.
echo ------------------------------------------------------------
echo  git-backup-bundle.bat
echo ------------------------------------------------------------
echo What it does:
echo   Creates a portable backup file containing the Git history.
echo   Saves it on your Desktop in:
echo     findreplacemark-git-backups
echo.
echo When to use it:
echo   Use this before risky changes.
e