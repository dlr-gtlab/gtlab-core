REM SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
REM
REM SPDX-License-Identifier: MPL-2.0+

REM THIS IS A SCRIPT TO PREPARE AND RUN UNITTESTS

SET PATH=%PATH%;build;lib;lib\%TARGETDIRNAME%;C:\Qt\Qt5.15\5.15.2\msvc2019_64\bin;%DEVTOOLS%\binDebug;

SET PROGRAMPATH="C:\Program Files\OpenCppCoverage"

%PROGRAMPATH%\OpenCppCoverage.exe --sources src --export_type=html --export_type=cobertura --modules lib\%TARGETDIRNAME%\* --excluded_sources build\%UNITTESTSNAME%.exe --excluded_sources %DEVTOOLS%\* -- build\%UNITTESTSNAME%.exe --gtest_output=xml:unittests.xml 
