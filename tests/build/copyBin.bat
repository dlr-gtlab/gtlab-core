@echo off

SET SERVERPATH=C:\deployment\%PLATFORMNAME%\%CATEGORY%\bin\%RELEASESTATUS%\%CI_COMMIT_TAG%

SET BINDIRECTORY=%SERVERPATH%\bin
SET DEBUGBINDIRECTORY=%SERVERPATH%\binDebug

if not exist "%BINDIRECTORY%" mkdir %BINDIRECTORY%\
if not exist "%DEBUGBINDIRECTORY%" mkdir %DEBUGBINDIRECTORY%\

if %PLATFORMNAME%==windows (

	copy /y build\GTlab.exe %BINDIRECTORY%\
	copy /y build\GTlabConsole.exe %BINDIRECTORY%\
	copy /y build\GTlabGui.dll %BINDIRECTORY%\

	copy /y build\GTlab.exe %DEBUGBINDIRECTORY%\
	copy /y build\GTlabConsole.exe %DEBUGBINDIRECTORY%\
	copy /y build\GTlabGui-d.dll %DEBUGBINDIRECTORY%\
)

if %PLATFORMNAME%==linux (

	copy /y build\GTlab %BINDIRECTORY%\
	copy /y build\GTlabConsole %BINDIRECTORY%\
	copy /y build\libGTlabGui.so* %BINDIRECTORY%\

	copy /y build\GTlab %DEBUGBINDIRECTORY%\
	copy /y build\GTlabConsole %DEBUGBINDIRECTORY%\
	copy /y build\libGTlabGui-d.so* %DEBUGBINDIRECTORY%\
)

