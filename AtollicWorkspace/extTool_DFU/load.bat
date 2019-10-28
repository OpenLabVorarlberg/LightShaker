@ echo off
echo Converting .hex file to .dfu ...
for /f %%a in ('dir /B /S *.hex') do %~dp0DfuFileMgr.exe %%a %%a.dfu >nul

echo Updating device firmware ...
for /f %%a in ('dir /B /S *.dfu') do %~dp0DfuSeCommand.exe -c -d --fn %%a -l

echo Done