@echo off
REM Download VS Code installer
curl -L "https://update.code.visualstudio.com/latest/win32-x64-user/stable" -o VSCodeSetup.exe

REM Install silently without auto-launch
VSCodeSetup.exe /silent /mergetasks=!runcode

REM Clean up installer
del VSCodeSetup.exe

echo Visual Studio Code installation completed.
pause