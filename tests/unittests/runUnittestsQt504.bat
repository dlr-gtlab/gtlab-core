REM THIS IS A SCRIPT TO PREPARE AND RUN UNITTESTS

SET DEVTOOLS=D:\GTlab-DevTools\ThirdPartyLibraries

SET PATH=%PATH%;C:\Qt\5.4\msvc2013_64\bin;%DEVTOOLS%\minpack\lib;%DEVTOOLS%\SplineLib\lib;%DEVTOOLS%\NLopt\lib;lib/core;D:\Programme\GitLab-Runner\builds\41kyzfh-\0\at-twk\gtlab-numerics\lib\numerics;D:\Programme\GitLab-Runner\builds\41kyzfh-\0\at-twk\gtlab-logging\lib\logging

REM build\GTlabUnitTest.exe --gtest_output=xml:unittests.xml 

D:\Programme\OpenCppCoverage\OpenCppCoverage.exe --sources src --export_type=html --export_type=cobertura --excluded_modules Qt --excluded_sources %DEVTOOLS%\* -- build\GTlabUnitTest.exe --gtest_output=xml:unittests.xml 

D:\Programme\GitLab-Runner\BadgeBuilder\BadgeGenerator.exe --cc .\GTlabUnitTestCoverage.xml .
