SET SERVERPATH=C:\deployment\%CI_PROJECT_NAME%\%CI_COMMIT_TAG%

if exist "%SERVERPATH%\include\%TARGETDIRNAME%\" rmdir /s /q %SERVERPATH%\include\%TARGETDIRNAME%\
if not exist "%SERVERPATH%\include\%TARGETDIRNAME%\" mkdir %SERVERPATH%\include\%TARGETDIRNAME%\
if exist "%SERVERPATH%\lib\%TARGETDIRNAME%\" rmdir /s /q %SERVERPATH%\lib\%TARGETDIRNAME%\
if not exist "%SERVERPATH%\lib\%TARGETDIRNAME%\" mkdir %SERVERPATH%\lib\%TARGETDIRNAME%\

if not exist "%SERVERPATH%\%BINDIRECTORY%" mkdir %SERVERPATH%\%BINDIRECTORY%\
if not exist "%SERVERPATH%\%DEBUGBINDIRECTORY%\" mkdir %SERVERPATH%\%DEBUGBINDIRECTORY%

for /r .\include %%f in (*.h) do @copy "%%f" %SERVERPATH%\include\%TARGETDIRNAME%\
for /r .\lib %%f in (*.lib) do @copy "%%f" %SERVERPATH%\lib\%TARGETDIRNAME%\
for /r .\lib %%f in (*-d.dll) do @copy "%%f" %SERVERPATH%\%DEBUGBINDIRECTORY%\
for /r .\lib %%i in (*.dll) do (echo %%i | findstr /r ".*-d.dll" > nul || copy %%i %SERVERPATH%\%BINDIRECTORY%\)
