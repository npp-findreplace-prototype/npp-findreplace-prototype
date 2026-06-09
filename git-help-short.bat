@echo off
setlocal EnableExtensions

cd /d "%~dp0"

echo(
echo ============================================================
echo  Git helper quick help
echo ============================================================
echo(
echo Folder:
echo   %CD%
echo(
echo Main idea:
echo   For normal code work, run build.bat.
echo   For uncertainty, run git-status-check.bat.
echo   For docs or script-only changes, run git-commit-and-push-now.bat.
echo(
echo ============================================================
echo  3-line descriptions
echo ============================================================

echo(
echo build.bat
echo   Builds the program and creates build/source snapshots after success.
echo   Commits changed project files and pushes them to GitHub.
echo   Use after normal code edits.

echo(
echo git-status-check.bat
echo   Shows local changes, branch, remote, and GitHub sync state.
echo   Tells you whether you are clean, ahead, behind, or both.
echo   Use whenever you are unsure what to do next.

echo(
echo git-get-latest.bat
echo   Gets the latest commits from GitHub safely.
echo   Only works when your local folder has no uncommitted changes.
echo   Use before working on another computer.

echo(
echo git-push-local.bat
echo   Pushes already-created local commits to GitHub.
echo   Does not commit new file changes.
echo   Use when status says you are ahead of GitHub.

echo(
echo git-commit-and-push-now.bat
echo   Commits current file changes with a message.
echo   Pushes that commit to GitHub.
echo   Use for README, docs, helper scripts, or non-build changes.

echo(
echo git-show-history.bat
echo   Shows recent commits in a readable graph.
echo   Helps confirm that your saves and builds were committed.
echo   Use when you want to see project history.

echo(
echo git-backup-bundle.bat
echo   Creates an offline Git backup bundle on your Desktop.
echo   Does not need internet or GitHub.
echo   Use before risky changes or moving computers.

echo(
echo github-verify-clone.bat
echo   Makes a temporary fresh clone from GitHub.
echo   Confirms GitHub has a usable copy of the project.
echo   Use after important pushes.

echo(
echo git-discard-local-changes-DANGEROUS.bat
echo   Throws away uncommitted local changes.
echo   Restores files back to the last commit.
echo   Use only when you are sure you want to abandon edits.

echo(
echo ============================================================
echo  1-line quick summary
echo ============================================================

echo(
echo build.bat - Build, snapshot, commit, and push after successful code changes.
echo git-status-check.bat - Check what state Git is in and what to do next.
echo git-get-latest.bat - Safely update from GitHub before starting work.
echo git-push-local.bat - Push local commits that already exist.
echo git-commit-and-push-now.bat - Manually commit and push non-build changes.
echo git-show-history.bat - Show recent commit history.
echo git-backup-bundle.bat - Create an offline backup of the Git repo.
echo github-verify-clone.bat - Test that GitHub can be cloned successfully.
echo git-discard-local-changes-DANGEROUS.bat - Throw away uncommitted local edits.
echo(

echo ============================================================
echo  Usual choices
echo ============================================================
echo(
echo Normal code work:
echo   build.bat
echo(
echo Unsure what to do:
echo   git-status-check.bat
echo(
echo Script or README edit:
echo   git-commit-and-push-now.bat
echo(
echo Before risky work:
echo   git-backup-bundle.bat
echo(

pause
exit /b 0