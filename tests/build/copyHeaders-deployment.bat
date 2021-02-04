@echo off

SET SERVERPATH=C:\deployment\%PLATFORMNAME%\%CATEGORY%\%CI_PROJECT_NAME%\%CI_COMMIT_TAG%

if %CATEGORY%==01_third_party (
    SET INCLUDEDIRECTORY=%SERVERPATH%\ThirdPartyLibraries\%TARGETDIRNAME%\include
	SET LIBDIRECTORY=%SERVERPATH%\ThirdPartyLibraries\%TARGETDIRNAME%\lib
    SET BINDIRECTORY=%SERVERPATH%\bin
	SET DEBUGBINDIRECTORY=%SERVERPATH%\binDebug
) else (
    SET INCLUDEDIRECTORY=%SERVERPATH%\include\%TARGETDIRNAME%
	SET LIBDIRECTORY=%SERVERPATH%\lib\%TARGETDIRNAME%

	if %CATEGORY%==02_core (
		SET BINDIRECTORY=%SERVERPATH%\bin
		SET DEBUGBINDIRECTORY=%SERVERPATH%\binDebug
	) else (
		SET BINDIRECTORY=%SERVERPATH%\bin\modules
		SET DEBUGBINDIRECTORY=%SERVERPATH%\binDebug\modules
	)
)

if exist "%INCLUDEDIRECTORY%\" rmdir /s /q %INCLUDEDIRECTORY%\
if not exist "%INCLUDEDIRECTORY%\" mkdir %INCLUDEDIRECTORY%\
if exist "%LIBDIRECTORY%\" rmdir /s /q %LIBDIRECTORY%\
if not exist "%LIBDIRECTORY%\" mkdir %LIBDIRECTORY%\

if not exist "%BINDIRECTORY%" mkdir %BINDIRECTORY%\
if not exist "%DEBUGBINDIRECTORY%" mkdir %DEBUGBINDIRECTORY%\

for /r .\include %%f in (*.h) do @copy "%%f" %INCLUDEDIRECTORY%\

if %PLATFORMNAME%==windows (
    for /r .\lib %%f in (*.lib) do @copy "%%f" %LIBDIRECTORY%\
    for /r .\lib %%f in (*-d.dll) do @copy "%%f" %DEBUGBINDIRECTORY%\
    for /r .\lib %%i in (*.dll) do (echo %%i | findstr /r ".*-d.dll" > nul || copy %%i %BINDIRECTORY%\)
)

if %PLATFORMNAME%==linux (
    for /r .\lib %%f in (*.so) do @copy "%%f" %LIBDIRECTORY%\
    for /r .\lib %%f in (*-d.so) do @copy "%%f" %DEBUGBINDIRECTORY%\
    for /r .\lib %%i in (*.so) do (echo %%i | findstr /r ".*-d.so" > nul || copy %%i %BINDIRECTORY%\)
)
