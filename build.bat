@echo off
for /f "usebackq delims=" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -property installationPath`) do set VS=%%i
call "%VS%\VC\Auxiliary\Build\vcvars64.bat" >nul
cd /d "%~dp0"
cl /nologo /O1 /GS- /W3 clicksound.c /Fe:clicksound.exe /link /SUBSYSTEM:WINDOWS user32.lib advapi32.lib winmm.lib
del clicksound.obj 2>nul
