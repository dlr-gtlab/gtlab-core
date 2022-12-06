@echo off

SET SERVERPATH=C:\deployment\%PLATFORMNAME%\%CATEGORY%\bin\%RELEASESTATUS%\%CI_COMMIT_TAG%

SET BINDIRECTORY=%SERVERPATH%\bin
SET BINDEBUGDIRECTORY=%SERVERPATH%\binDebug

if not exist "%BINDIRECTORY%" mkdir %BINDIRECTORY%\
if not exist "%BINDEBUGDIRECTORY%" mkdir %BINDEBUGDIRECTORY%\

if %PLATFORMNAME%==windows (

	copy /y pkg\bin\GTlab.exe %BINDIRECTORY%\
	copy /y pkg\bin\GTlabConsole.exe %BINDIRECTORY%\
    copy /y pkg\binDebug\GTlab.exe %BINDEBUGDIRECTORY%\
	copy /y pkg\binDebug\GTlabConsole.exe %BINDEBUGDIRECTORY%\
)

if %PLATFORMNAME%==linux (

	copy /y pkg\bin\GTlab %BINDIRECTORY%\
	copy /y pkg\bin\GTlabConsole %BINDIRECTORY%\
	copy /y pkg\binDebug\GTlab %BINDEBUGDIRECTORY%\
	copy /y pkg\binDebug\GTlabConsole %BINDEBUGDIRECTORY%\
)

