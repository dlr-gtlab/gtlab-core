@echo off

SET SERVERPATH=C:\deployment\%PLATFORMNAME%\%CATEGORY%\bin\%RELEASESTATUS%\%CI_COMMIT_TAG%

SET BINDIRECTORY=%SERVERPATH%\%DESTDIR%

if not exist "%BINDIRECTORY%" mkdir %BINDIRECTORY%\

if %PLATFORMNAME%==windows (

	copy /y build\GTlab.exe %BINDIRECTORY%\
	copy /y build\GTlabConsole.exe %BINDIRECTORY%\
	copy /y build\GTlabGui*.dll %BINDIRECTORY%\
)

if %PLATFORMNAME%==linux (

	copy /y build\GTlab %BINDIRECTORY%\
	copy /y build\GTlabConsole %BINDIRECTORY%\
	copy /y build\libGTlabGui*.so* %BINDIRECTORY%\
)

