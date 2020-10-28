REM THIS IS A SCRIPT TO PREPARE AND RUN UNITTESTS

SET PATH=%PATH%;lib;lib\core;build;C:\Qt\5.12.6\msvc2017_64\bin;%DEVTOOLS%\binDebug;

REM build\DLRperformanceUnitTest.exe --gtest_output=xml:unittests.xml 

SET PROGRAMPATH="C:\Program Files\OpenCppCoverage"

%PROGRAMPATH%\OpenCppCoverage.exe --sources src --export_type=html --export_type=cobertura --modules lib\%TARGETDIRNAME%\* --excluded_sources build\%UNITTESTSNAME%.exe --excluded_sources %DEVTOOLS%\* -- build\%UNITTESTSNAME%.exe --gtest_output=xml:unittests.xml 
