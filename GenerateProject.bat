@echo off
echo ==== Generating Project ====
echo.
CALL Vendors\premake\Bin\premake5.exe vs2019
echo.
PAUSE