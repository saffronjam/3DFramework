@echo off
echo ==== Generating Project ====
echo.
pushd %~dp0\..\
CALL Dependencies\Premake\Bin\Premake5.exe vs2019
echo.
PAUSE