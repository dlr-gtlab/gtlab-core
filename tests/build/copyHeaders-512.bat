SET NIGHTLYBUILD=G:\AT-TW\GTlab\Nightly_Builds_512

REM for /r .\src\ %%f in (*.h) do @copy "%%f" D:\devel\GTlab-NightlyBuild\include\numerics\
if not exist "%NIGHTLYBUILD%\include\core\" mkdir %NIGHTLYBUILD%\include\core\
if not exist "%NIGHTLYBUILD%\lib\core\" mkdir %NIGHTLYBUILD%\lib\core\

for /r .\include %%f in (*.h) do @copy "%%f" %NIGHTLYBUILD%\include\core\
for /r .\lib %%f in (*.dll) do @copy "%%f" %NIGHTLYBUILD%\lib\core\
for /r .\lib %%f in (*.lib) do @copy "%%f" %NIGHTLYBUILD%\lib\core\
for /r .\lib %%f in (*.exe) do @copy "%%f" %NIGHTLYBUILD%\lib\core\