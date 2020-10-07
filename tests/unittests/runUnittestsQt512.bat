REM THIS IS A SCRIPT TO PREPARE AND RUN UNITTESTS

SET PATH=%PATH%;C:\Qt\5.12.6\msvc2017_64\bin
SET PATH=%PATH%;build;lib\core
SET PATH=%PATH%;%DEVTOOLS%\binDebug;


SET PROGRAMPATH="C:\Program Files\OpenCppCoverage"

%PROGRAMPATH%\OpenCppCoverage.exe --sources src --export_type=html --export_type=cobertura --excluded_modules Qt --excluded_sources %DEVTOOLS%\* -- build\GTlabUnitTest.exe --gtest_output=xml:unittests.xml 
