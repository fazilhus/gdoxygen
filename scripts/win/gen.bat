@echo off
pushd %~dp0\..\..\
call ext\premake\premake5-win.exe vs2022
popd
PAUSE