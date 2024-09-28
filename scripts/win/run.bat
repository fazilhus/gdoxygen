@echo off
set arg1=%1
pushd %~dp0\..\..\
call build\bin\Release-windows-x86_64\Core\Core.exe %arg1%
popd
PAUSE