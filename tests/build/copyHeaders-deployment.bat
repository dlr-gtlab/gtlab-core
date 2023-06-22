REM SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
REM
REM SPDX-License-Identifier: MPL-2.0+

REM @echo off

SET SERVERPATH=C:\deployment\%PLATFORMNAME%\%CATEGORY%\%CI_PROJECT_NAME%\%RELEASESTATUS%\%CI_COMMIT_TAG%

rmdir /s /q %SERVERPATH%
mkdir %SERVERPATH%

robocopy pkg %SERVERPATH% /MIR


if %PLATFORMNAME%==windows (

	del %SERVERPATH%\bin\GTlab.exe /F /Q
    del %SERVERPATH%\bin\GTlabConsole.exe /F /Q
    del %SERVERPATH%\binDebug\GTlab.exe /F /Q
    del %SERVERPATH%\binDebug\GTlabConsole.exe /F /Q
)

if %PLATFORMNAME%==linux (
	del %SERVERPATH%\bin\GTlab /F /Q
    del %SERVERPATH%\bin\GTlabConsole /F /Q
	del %SERVERPATH%\binDebug\GTlab /F /Q
    del %SERVERPATH%\binDebug\GTlabConsole /F /Q
)

