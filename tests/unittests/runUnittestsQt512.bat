REM THIS IS A SCRIPT TO PREPARE AND RUN UNITTESTS

SET DEVTOOLS=C:\devel\GTlab-DevTools\ThirdPartyLibraries

SET PATH=%PATH%;C:\Qt\5.12.6\msvc2017_64\bin;%DEVTOOLS%\minpack\lib;%DEVTOOLS%\SplineLib\lib;%DEVTOOLS%\NLopt\lib;lib/core;C:\devel\gitlab-runner\builds\SuMtsyPf\0\at-twk\gtlab-numerics\lib\numerics;C:\devel\gitlab-runner\builds\SuMtsyPf\0\at-twk\gtlab-logging\lib\logging

build\GTlabUnitTest.exe --gtest_output=xml:unittests.xml 

REM D:\Programme\OpenCppCoverage\OpenCppCoverage.exe --sources src --export_type=html --export_type=cobertura --excluded_modules Qt --excluded_sources %DEVTOOLS%\* -- build\GTlabUnitTest.exe --gtest_output=xml:unittests.xml 

REM D:\Programme\GitLab-Runner\BadgeBuilder\BadgeGenerator.exe --cc .\GTlabUnitTestCoverage.xml .
