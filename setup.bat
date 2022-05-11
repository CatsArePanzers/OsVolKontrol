md %~dp0bin
setx /M path "%path%;%~dp0bin"
curl.exe -o %~dp0nircmd-x64.zip https://www.nirsoft.net/utils/nircmd-x64.zip
powershell -command "Expand-Archive -Force %~dp0nircmd-x64.zip '%~dp0'"
del %~dp0NirCmd.chm
del %~dp0nircmdc.exe
del %~dp0nircmd-x64.zip
move %~dp0nircmd.exe %~dp0bin
call %~dp0compile.bat
move %~dp0volKontrol.exe %~dp0bin
xcopy %~dp0volKontrol.bat %~dp0bin\
nircmd elevate volKontrol.bat






