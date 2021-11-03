@echo off

echo Generating Project
echo.

pushd %~dp0\..\

set "write=powershell write-host -background"
set Script=ThirdParty\Premake\Bin\premake5.exe vs2022

call %Script%

echo.

if %errorlevel% gtr 0 (
%write% red Failed
pause
) else (
%write% green Success
timeout /t 1
pause
)

echo.
